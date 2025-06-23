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

#include "XYViewerCanvas.h"
#include "XYViewer.h"
using namespace XYViewerPlugin;

XYViewerCanvas::XYViewerCanvas(XYViewer* processor_)
    : m_processor(processor_)
{
    m_plt.xlabel("X");
    m_plt.ylabel("Y");
    m_plt.title("X vs Y");
    m_plt.setInteractive(InteractivePlotMode::OFF);
    m_plt.setBackgroundColour(Colours::darkolivegreen);
    addAndMakeVisible(m_plt);
    m_plt.setBounds(1, 1, 640  , 480);
}

XYViewerCanvas::~XYViewerCanvas() {}

void XYViewerCanvas::resized() {}

void XYViewerCanvas::refreshState() {}

// void XYViewerCanvas::update()
//{

//}

void XYViewerCanvas::refresh() {}

void XYViewerCanvas::paint(Graphics& g)
{
    g.fillAll(Colours::black);
}