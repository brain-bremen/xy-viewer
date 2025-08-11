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

#include <random>
using namespace XYViewerPlugin;

XyLineFading::XyLineFading (std::vector<float>&& x, std::vector<float>&& y)
    : ::XYLine (std::move (x), std::move (y)), X (std::move (x)), Y (std::move (y))
{
}
//XyLineFading::XyLineFading (std::vector<float> x, std::vector<float> y)
//: ::XYLine (x, y)
//{
//XYLine::setColour (c);
//XYLine::setWidth (maxWidth);
//XYLine::setOpacity (maxOpacity);
//XYLine::setType (PlotType::LINE);
//}
void XyLineFading::draw (Graphics& g, XYRange& range_, int plotWidth, int plotHeight)
{
    if (type != PlotType::LINE)
        return;
    //XYLine::draw (g, range, plotWidth, plotHeight);

    float yrange = range.ymax - range.ymin;
    float xrange = range.xmax - range.xmin;

    float startOpacity = 0.2f;
    float endOpacity = opacity;
    float startWidth = width * 0.2f;
    float endWidth = width;

    if (yrange < 1e-6 || xrange < 1e-6)
        return;

    for (int i = 1; i < x.size(); i++)
    {
        const float t = static_cast<float> (i) / static_cast<float> (x.size() - 1); // Progress from 0 to 1

        float segOpacity = startOpacity + t * (endOpacity - startOpacity);
        float segWidth = startWidth + t * (endWidth - startWidth);
        float x_start = (x[i - 1] - range.xmin) / xrange;
        float x_end = (x[i] - range.xmin) / xrange;

        g.setColour (colour.withAlpha (segOpacity));
        if ((x_start < 0 && x_end < 0) || (x_start > 1 && x_end > 1))
            continue;

        if (i >= y.size())
            continue;

        float y_start = (y[i - 1] - range.ymin) / yrange;
        float y_end = (y[i] - range.ymin) / yrange;

        if ((y_start < 0 && y_end < 0) || (y_start > 1 && y_end > 1))
            continue;

        if (true)
        {
            x_start = x_start * plotWidth;
            x_end = x_end * plotWidth;
        }
        //else
        //{
        //	x_start = plotWidth - x_start * plotWidth;
        //	x_end = plotWidth - x_end * plotWidth;
        //}

        if (true)
        {
            y_start = plotHeight - y_start * plotHeight;
            y_end = plotHeight - y_end * plotHeight;
        }

        //else
        //	y_start = y_start * plotHeight;

        g.drawLine (x_start,
                    y_start,
                    x_end,
                    y_end,
                    segWidth);
    }
}
void XYFadingTracePlot::plot (std::vector<float> x, std::vector<float> y, Colour c, float width, float opacity, PlotType type)
{
    ::InteractivePlot::plot (std::move (x), std::move (y), c, width, opacity, type);
}
void XYFadingTracePlot::plot (XyLineFading* line) const
{
    if (! line)
        return;
    drawComponent->add (line);
}
XYViewerCanvas::XYViewerCanvas (XYViewer* processor_)
    : m_processor (processor_)
{
    m_plt.xlabel ("X");
    m_plt.ylabel ("Y");
    m_plt.title ("X vs Y");
    m_plt.setInteractive (InteractivePlotMode::ON);
    m_plt.setBackgroundColour (Colours::darkolivegreen);
    addAndMakeVisible (&m_plt);
    // m_plt.setBounds(1, 1, 640, 480);
    //m_plt.setBounds(50, 40, 120, 20);
    m_plt.setBounds (50, 50, 800, 500);
    m_range = XYRange { -5.0f, 5.0, -5.0f, 5.0f };
    m_plt.showGrid (true);
    m_plt.setRange (m_range);
}

XYViewerCanvas::~XYViewerCanvas() {}

void XYViewerCanvas::resized() {}

void XYViewerCanvas::refreshState() {}

// void XYViewerCanvas::update()
//{

//}

void XYViewerCanvas::refresh()
{
    std::vector<float> x;
    std::vector<float> y;

    //// WIP: Draw random points for now
    //int nPoints = 100; // or use a member variable if defined elsewhere
    //std::random_device rd;
    //std::mt19937 gen (rd());
    //std::uniform_real_distribution<float> dist (0.0f, 1.0f);

    //for (int i = 0; i < nPoints; ++i)
    //{
    //    x.push_back (dist (gen) + i);
    //    y.push_back (dist (gen) + i);
    //}
    auto retentionPeriodMs = m_processor->getParameter (XYViewerPlugin::ParameterNames::keep_window_length)->getValue();
    m_processor->getXYData (x, y, m_retention_period_ms);

    m_plt.clear();
    XyLineFading* line = new XyLineFading (std::move(x), std::move(y));

    line->setColour (Colours::black);
    line->setWidth (2.0f);
    line->setOpacity (1.0f);
    line->setType (PlotType::LINE);
    //line->setColour (c)
    //m_plt.plot (x, y, Colours::black, 2.0, 0.8f, PlotType::LINE);

    m_plt.plot (line);
}

void XYViewerCanvas::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}