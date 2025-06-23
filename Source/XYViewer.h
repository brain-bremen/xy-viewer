/*
    ------------------------------------------------------------------

    This file is part of the XY Viewer Plugin for the Open Ephys GUI
    Copyright (C) 2022 Open Ephys
    Copyright (C) 2025 Joscha Schmiedt <schmiedt@uni-bremen.de>

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef XYVIEWER_H_DEFINED
#define XYVIEWER_H_DEFINED

#include <ProcessorHeaders.h>

namespace XYViewerPlugin
{

class XYViewerCanvas;

struct ParameterNames
{
    static constexpr auto keep_window_length = "keep_window_length";
};

class XYViewer : public GenericProcessor
{
  public:
    XYViewer();
    ~XYViewer() override;

    /** If the processor has a custom editor, this method must be defined to instantiate it. */
    AudioProcessorEditor* createEditor() override;

    /** Called every time the settings of an upstream plugin are changed.
        Allows the processor to handle variations in the channel configuration or any other
       parameter passed through signal chain. The processor can use this function to modify channel
       objects that will be passed to downstream plugins. */
    void updateSettings() override;

    /** Defines the functionality of the processor.
        The process method is called every time a new data buffer is available.
        Visualizer plugins typically use this method to send data to the canvas for display purposes
     */
    void process(AudioBuffer<float>& buffer) override;

    bool startAcquisition() override;
    bool stopAcquisition() override;
    
    /** Handles events received by the processor
        Called automatically for each received event whenever checkForEvents() is called from
        the plugin's process() method */
    void handleTTLEvent(TTLEventPtr event) override;

    /** Handles spikes received by the processor
        Called automatically for each received spike whenever checkForEvents(true) is called from
        the plugin's process() method */
    void handleSpike(SpikePtr spike) override;

    /** Saving custom settings to XML. This method is not needed to save the state of
        Parameter objects */
    void saveCustomParametersToXml(XmlElement* parentElement) override;

    /** Load custom settings from XML. This method is not needed to load the state of
        Parameter objects*/
    void loadCustomParametersFromXml(XmlElement* parentElement) override;

    Array<String> getChannelsForStream(uint16 streamdId) const;
    void parameterValueChanged(Parameter*) override;
    void setActiveChannel(uint16 streamId, String name);

    void setCanvas(XYViewerCanvas* canvas) noexcept { m_canvas = canvas; }

  private:
    /** Generates an assertion if this class leaks */
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XYViewer);

    XYViewerCanvas* m_canvas = nullptr;

    struct ContinuousChannelInfo
    {
        String name;
        uint16 streamID = 0;
        float sampleRate = 0.0f;
        bool isActive = false;
    };
    std::vector<ContinuousChannelInfo> m_channels;
    std::unordered_map<const ContinuousChannel*, ContinuousChannelInfo*> m_channelMap;
};

} // namespace XYViewerPlugin
#endif // XYVIEWER_H_DEFINED