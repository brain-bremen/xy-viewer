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

#include "XYViewer.h"
#include "CircularXYBuffer.h"
#include "XYViewerCanvas.h"
#include "XYViewerEditor.h"

using namespace XYViewerPlugin;

XYViewer::XYViewer()
    : GenericProcessor ("XY Viewer"), m_xyBuffer (30000 * 60)
{
    m_channels.reserve (384);
    addIntParameter (Parameter::PROCESSOR_SCOPE, ParameterNames::keep_window_length, "Retention (ms)", "Duration of trace to keep in ms", 2000, 100, 10000);
}

XYViewer::~XYViewer() = default;

AudioProcessorEditor* XYViewer::createEditor()
{
    editor = std::make_unique<XYViewerEditor> (this);
    return editor.get();
}

void XYViewer::updateSettings()
{
    m_channels.clear();
    for (ContinuousChannel* contChan : continuousChannels)
    {
        String streamName;
        if (DataStream* stream = getDataStream (contChan->getStreamId()))
            streamName = stream->getName();
        m_channels.push_back ({ contChan->getName(), streamName, contChan->getStreamId(), contChan->getSampleRate() });
    }

    resolveActiveChannels();

    if (m_canvas)
    {
        parameterValueChanged (getParameter (ParameterNames::keep_window_length));
    }
}

void XYViewer::resolveActiveChannels()
{
    auto findChannelIndex = [this] (const String& name, const String& streamName) -> int
    {
        int index = 0;
        for (const auto& channel : m_channels)
        {
            if (channel.name.equalsIgnoreCase (name) && (streamName.isEmpty() || channel.streamName.equalsIgnoreCase (streamName)))
                return index;
            index++;
        }
        return -1;
    };

    if (m_xName.isNotEmpty())
    {
        int index = findChannelIndex (m_xName, m_xStreamName);
        if (index >= 0)
            m_xChannelIndex = index;
    }
    else if (! m_channels.empty())
    {
        m_xChannelIndex = 0;
        m_xName = m_channels[0].name;
        m_xStreamName = m_channels[0].streamName;
    }

    if (m_yName.isNotEmpty())
    {
        int index = findChannelIndex (m_yName, m_yStreamName);
        if (index >= 0)
            m_yChannelIndex = index;
    }
    else if (m_channels.size() > 1)
    {
        m_yChannelIndex = 1;
        m_yName = m_channels[1].name;
        m_yStreamName = m_channels[1].streamName;
    }

    if (m_canvas)
        m_canvas->setChannelNames (m_xName, m_yName);
}

void XYViewer::process (AudioBuffer<float>& buffer)
{
    //checkForEvents (true);
    // Only push if both X and Y channels are valid
    if (m_xChannelIndex >= 0 && m_yChannelIndex >= 0 && m_xChannelIndex < buffer.getNumChannels() && m_yChannelIndex < buffer.getNumChannels())
    {
        const auto nSamples = buffer.getNumSamples();
        const float* x = buffer.getReadPointer (m_xChannelIndex);
        const float* y = buffer.getReadPointer (m_yChannelIndex);
        m_xyBuffer.push (x, y, nSamples);
    }
    //for (int chan = 0; chan < buffer.getNumChannels(); ++chan)
    //{
    //const uint16 streamId = continuousChannels[chan]->getStreamId();
    //const uint32 nSamples = getNumSamplesInBlock (streamId);
    //String streamKey = getDataStream (streamId)->getKey();
    //}
}

bool XYViewer::startAcquisition()
{
    XYViewerEditor* editor_ = dynamic_cast<XYViewerEditor*> (getEditor());
    if (! editor_)
        return false;
    editor_->enable();

    return true;
}

bool XYViewer::stopAcquisition()
{
    XYViewerEditor* editor_ = dynamic_cast<XYViewerEditor*> (getEditor());
    if (! editor_)
        return false;
    editor_->disable();

    return true;
}

void XYViewer::handleTTLEvent (TTLEventPtr event) {}

void XYViewer::saveCustomParametersToXml (XmlElement* parentElement)
{
    XmlElement* xySelection = parentElement->createNewChildElement ("XY_SELECTION");
    xySelection->setAttribute ("x_channel", m_xName);
    xySelection->setAttribute ("x_stream", m_xStreamName);
    xySelection->setAttribute ("y_channel", m_yName);
    xySelection->setAttribute ("y_stream", m_yStreamName);
}

void XYViewer::loadCustomParametersFromXml (XmlElement* parentElement)
{
    if (XmlElement* xySelection = parentElement->getChildByName ("XY_SELECTION"))
    {
        m_xName = xySelection->getStringAttribute ("x_channel");
        m_xStreamName = xySelection->getStringAttribute ("x_stream");
        m_yName = xySelection->getStringAttribute ("y_channel");
        m_yStreamName = xySelection->getStringAttribute ("y_stream");
        resolveActiveChannels();
    }
}

Array<String> XYViewer::getChannelsForStream (uint16 streamdId) const
{
    Array<String> channelsForStream;
    for (const auto& channel : m_channels)
    {
        if (channel.streamID == streamdId)
        {
            channelsForStream.add (channel.name);
        }
    }

    return channelsForStream;
}

void XYViewer::parameterValueChanged (Parameter* parameter)
{
    GenericProcessor::parameterValueChanged (parameter);
    if (parameter->getName().equalsIgnoreCase (ParameterNames::keep_window_length))
    {
        int keep_window_length = static_cast<int> (parameter->getValue());
        if (m_canvas)
            m_canvas->setRetentionPeriodMs (keep_window_length);
    }
}

void XYViewer::setActiveXChannel (uint16 streamId,  const String& name)
{
    int index = 0;
    for (auto& channel : m_channels)
    {
        if (channel.name.equalsIgnoreCase (name) && channel.streamID == streamId)
        {
            m_xChannelIndex = index;
            m_xName = name;
            m_xStreamName = channel.streamName;
            if (m_canvas)
            {
                m_canvas->setChannelNames (m_xName, m_yName);
            }
            return;
        }
        index++;
    }
}
void XYViewer::setActiveYChannel (uint16 streamId,  const String& name)
{
    int index = 0;
    for (auto& channel : m_channels)
    {
        if (channel.name.equalsIgnoreCase (name) && channel.streamID == streamId)
        {
            m_yChannelIndex = index;
            m_yName = name;
            m_yStreamName = channel.streamName;
            if (m_canvas)
            {
                m_canvas->setChannelNames (m_xName, m_yName);
            }
            return;
        }
        index++;
    }
}

// Get the latest X and Y data for plotting (thread-safe copy)
void XYViewer::getXYData (std::vector<float>& x, std::vector<float>& y, int retentionMs)
{
    // Use the sample rate of the X channel (if available)
    float sampleRate = 30000.0f;
    if (m_xChannelIndex >= 0 && m_xChannelIndex < (int) m_channels.size())
        sampleRate = m_channels[m_xChannelIndex].sampleRate;
    int nSamples = int (sampleRate * retentionMs / 1000.0f);
    m_xyBuffer.getLatest (x, y, nSamples);
}
