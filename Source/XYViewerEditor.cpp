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

#include "XYViewerEditor.h"
#include "XYViewer.h"
#include "XYViewerCanvas.h"
using namespace XYViewerPlugin;

XYViewerEditor::XYViewerEditor(GenericProcessor* p)
    : VisualizerEditor(p, "XY Viewer", 210)
{
    xChannelList = std::make_unique<ComboBox>("X Channel List");
    xChannelList->addListener(this);
    xChannelList->setBounds(50, 40, 120, 20);
    addAndMakeVisible(xChannelList.get());

    yChannelList = std::make_unique<ComboBox>("Y Channel List");
    yChannelList->addListener(this);
    yChannelList->setBounds(50, 70, 120, 20);
    addAndMakeVisible(yChannelList.get());

    addTextBoxParameterEditor(Parameter::PROCESSOR_SCOPE, ParameterNames::keep_window_length, 50,
                              95);
}

Visualizer* XYViewerEditor::createNewCanvas()
{
    XYViewer* xyViewerNode = dynamic_cast<XYViewer*>(getProcessor());
    if (!xyViewerNode) return nullptr;

    XYViewerCanvas* xyViewerCanvas = new XYViewerCanvas(xyViewerNode);
    xyViewerNode->setCanvas(xyViewerCanvas);

    xyViewerCanvas->setRetentionPeriodMs(
        xyViewerNode->getParameter(ParameterNames::keep_window_length)->getValue());
    xyViewerNode->setActiveChannel(selectedStream, xChannelList->getText());

    return xyViewerCanvas;
}

void XYViewerEditor::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (!comboBoxThatHasChanged) return;
    if (comboBoxThatHasChanged == xChannelList.get() && comboBoxThatHasChanged->getNumItems() > 0)
    {
        XYViewer* xyViewerNode = dynamic_cast<XYViewer*>(getProcessor());
        if (!xyViewerNode) return;

        xyViewerNode->setActiveChannel(selectedStream, comboBoxThatHasChanged->getText());
        // xyViewerNode->setActiveYChannel(selectedStream, comboBoxThatHasChanged->getText());
    }
}

void XYViewerEditor::selectedStreamHasChanged()
{
    XYViewer* xyViewerNode = dynamic_cast<XYViewer*>(getProcessor());
    if (xyViewerNode == nullptr) return;
    if (selectedStream == 0) return;

    xChannelList->clear();
    yChannelList->clear();

    Array<String> currentChannels = xyViewerNode->getChannelsForStream(selectedStream);
    int id = 0;
    for (const auto& channel : currentChannels)
    {
        xChannelList->addItem(channel, ++id);
        yChannelList->addItem(channel, ++id);
    }
    xChannelList->setSelectedId(1, sendNotification);
    yChannelList->setSelectedId(2, sendNotification);
}
