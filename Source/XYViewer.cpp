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
#include "XYViewerCanvas.h"
#include "XYViewerEditor.h"

using namespace XYViewerPlugin;

XYViewer::XYViewer()
    : GenericProcessor("XY Viewer")
{
    m_channels.reserve(384);
    addIntParameter(Parameter::PROCESSOR_SCOPE, ParameterNames::keep_window_length,
                    "Window Length (ms)", "Duration of trace to keep in ms", 2000, 100, 60000);
}

XYViewer::~XYViewer() {}

AudioProcessorEditor* XYViewer::createEditor()
{
    editor = std::make_unique<XYViewerEditor>(this);
    return editor.get();
}

void XYViewer::updateSettings()
{
    m_channels.clear();
    for (ContinuousChannel* contChan : continuousChannels)
    {
        m_channels.emplace_back(contChan->getName(), contChan->getStreamId(),
                                contChan->getSampleRate());
    }

    if (m_canvas)
    {
        parameterValueChanged(getParameter(ParameterNames::keep_window_length));
    }
}

void XYViewer::process(AudioBuffer<float>& buffer)
{
    checkForEvents(true);
}

void XYViewer::handleTTLEvent(TTLEventPtr event) {}

void XYViewer::handleSpike(SpikePtr spike) {}

// void XYViewerPlugin::handleBroadcastMessage(String message)
//{

//}

void XYViewer::saveCustomParametersToXml(XmlElement* parentElement) {}

void XYViewer::loadCustomParametersFromXml(XmlElement* parentElement) {}

Array<String> XYViewer::getChannelsForStream(uint16 streamdId) const
{
    Array<String> channelsForStream;
    for (const auto& channel : m_channels)
    {
        if (channel.streamID == streamdId)
        {
            channelsForStream.add(channel.name);
        }
    }

    return channelsForStream;
}

void XYViewer::parameterValueChanged(Parameter* parameter)
{
    GenericProcessor::parameterValueChanged(parameter);
    if (parameter->getName().equalsIgnoreCase(ParameterNames::keep_window_length))
    {
        int keep_window_length = static_cast<int>(parameter->getValue());
        if (m_canvas) m_canvas->setRetentionPeriodMs(keep_window_length);
    }
}

void XYViewer::setActiveChannel(uint16 streamId, String name)
{
    for (auto& channel : m_channels)
    {
        if (channel.name.equalsIgnoreCase(name) && channel.streamID == streamId)
        {
            channel.isActive = true;
            if (m_canvas)
            {
                m_canvas->setPlotTitle(channel.name);
            }
        }

        else
        {
            channel.isActive = false;
        }
    }
}
