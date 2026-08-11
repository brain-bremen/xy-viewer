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

XyLineFading::XyLineFading (std::vector<float>&& x, std::vector<float>&& y)
    : ::XYLine (std::move (x), std::move (y))
{
}

void XyLineFading::draw (Graphics& g, XYRange& range_, int plotWidth, int plotHeight)
{
    if (type != PlotType::LINE)
        return;

    // Use the view range passed in by DrawComponent (reflects current pan/zoom),
    // not the inherited `range` member (which XYLine's constructor sets to this
    // line's own data bounds) - otherwise pan/zoom has no visible effect.
    float yrange = range_.ymax - range_.ymin;
    float xrange = range_.xmax - range_.xmin;

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
        float x_start = (x[i - 1] - range_.xmin) / xrange;
        float x_end = (x[i] - range_.xmin) / xrange;

        g.setColour (colour.withAlpha (segOpacity));
        if ((x_start < 0 && x_end < 0) || (x_start > 1 && x_end > 1))
            continue;

        if (i >= y.size())
            continue;

        float y_start = (y[i - 1] - range_.ymin) / yrange;
        float y_end = (y[i] - range_.ymin) / yrange;

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
    // Real bounds are assigned by resized() once the canvas is sized by its parent.
    m_range = XYRange { -5.0f, 5.0, -5.0f, 5.0f };
    m_plt.showGrid (true);
    m_plt.setRange (m_range);
}

XYViewerCanvas::~XYViewerCanvas() {}

void XYViewerCanvas::resized()
{
    m_plt.setBounds (getLocalBounds().reduced (10));
}

void XYViewerCanvas::refreshState() {}

void XYViewerCanvas::setChannelNames (const String& xName, const String& yName)
{
    m_plt.xlabel (xName);
    m_plt.ylabel (yName);
    m_plt.title (yName + " vs " + xName);
    m_needsAutoRange = true;
}

void XYViewerCanvas::refresh()
{
    std::vector<float> x;
    std::vector<float> y;

    m_processor->getXYData (x, y, m_retention_period_ms);

    m_plt.clear();
    XyLineFading* line = new XyLineFading (std::move (x), std::move (y));

    line->setColour (Colours::black);
    line->setWidth (2.0f);
    line->setOpacity (1.0f);
    line->setType (PlotType::LINE);

    m_plt.plot (line);

    // Fit the view to the data once, the first time real data arrives (or right
    // after the selected channels change) - after that, panning/zooming is left
    // entirely up to the user. The "Rescale" button lets them re-fit at any time.
    if (m_needsAutoRange && line->x.size() > 1)
    {
        XYRange bounds = line->getBounds();
        if ((bounds.xmax - bounds.xmin) > 1e-6f && (bounds.ymax - bounds.ymin) > 1e-6f)
        {
            m_plt.setRange (bounds);
            m_needsAutoRange = false;
        }
    }
}

void XYViewerCanvas::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}