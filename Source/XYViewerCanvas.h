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

#ifndef XYVIEWERCANVAS_H_INCLUDED
#define XYVIEWERCANVAS_H_INCLUDED

#include <VisualizerWindowHeaders.h>

namespace XYViewerPlugin
{
class XYViewer;

class XyLineFading : public ::XYLine
{
public:
    //XyLineFading (std::vector<float> x, std::vector<float> y, Colour c, float maxWidth, float maxOpacity);
    XyLineFading (std::vector<float> x, std::vector<float> y);
    void draw (Graphics& g, XYRange& range, int width, int height) override;
};

class XYFadingTracePlot : public ::InteractivePlot
{
public:
    void plot (std::vector<float> x, std::vector<float> y, Colour c, float width, float opacity, PlotType type) override;
    void plot (XyLineFading* line) const;
};

class XYViewerCanvas : public Visualizer
{
public:
    XYViewerCanvas (XYViewer* processor);
    ~XYViewerCanvas() override;

    /** Updates boundaries of sub-components whenever the canvas size changes */
    void resized() override;

    /** Called when the visualizer's tab becomes visible again */
    void refreshState() override;

    /** Updates settings */
    // void update() override;

    /** Called instead of "repaint()" to avoid re-painting sub-components*/
    void refresh() override;

    /** Draws the canvas background */
    void paint (Graphics& g) override;

    void setRetentionPeriodMs (int retentionPeriod) { m_retention_period_ms = retentionPeriod; }
    void setPlotTitle (const String& title) { m_plt.setTitle (title); }

private:
    /** Pointer to the processor class */
    XYViewer* m_processor;

    int m_retention_period_ms = 10000;

    /** Class for plotting data */
    XYFadingTracePlot m_plt;
    XYRange m_range;

    /** Generates an assertion if this class leaks */
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYViewerCanvas);
};

} // namespace XYViewerPlugin
#endif // XYVIEWERCANVAS_H_INCLUDED