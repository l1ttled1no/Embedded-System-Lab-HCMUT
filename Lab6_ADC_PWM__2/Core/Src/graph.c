#include "graph.h"
#include <string.h>
#include <stdlib.h>
#include <uart.h>
#include <stdio.h>

/* color choices (use values from Inc/lcd.h) */
#define GRAPH_BG_COLOR    BLACK
#define GRAPH_GRID_COLOR  GRAY
#define GRAPH_LINE_COLOR  GREEN
#define GRAPH_TEXT_COLOR  WHITE
#define GRAPH_AXIS_COLOR  WHITE
#define GRAPH_VALUE_COLOR YELLOW

// ...existing code...

void graph_render(void)
{
    /* clear graph background */
    lcd_Fill(g_x, g_y, g_x + g_w, g_y + g_h, GRAPH_BG_COLOR);

    /* Title above graph */
    lcd_ShowStr(g_x + 30, g_y - 25, "Power vs Time", GRAPH_TEXT_COLOR, BLACK, 16, 0);

    /* margins for axes labels */
    const uint8_t margin_left = 35;
    const uint8_t margin_bottom = 20;
    const uint8_t margin_top = 15;
    const uint8_t margin_right = 10;
    
    uint16_t gx0 = g_x + margin_left;
    uint16_t gy0 = g_y + margin_top;
    uint16_t gw = g_w - margin_left - margin_right;
    uint16_t gh = g_h - margin_top - margin_bottom;

    /* draw axes */
    // Y-axis
    lcd_DrawLine(gx0, gy0, gx0, gy0 + gh, GRAPH_AXIS_COLOR);
    // X-axis
    lcd_DrawLine(gx0, gy0 + gh, gx0 + gw, gy0 + gh, GRAPH_AXIS_COLOR);
    
    /* Y-axis label */
    lcd_ShowStr(g_x + 2, gy0 + gh/2 - 8, "W", GRAPH_TEXT_COLOR, BLACK, 16, 0);
    
    /* X-axis label */
    lcd_ShowStr(gx0 + gw/2 - 15, gy0 + gh + 5, "Time", GRAPH_TEXT_COLOR, BLACK, 12, 0);

    if (sample_count == 0) {
        /* no data, show hint */
        lcd_ShowStr(gx0 + 30, gy0 + gh/2 - 8, "No data", GRAPH_TEXT_COLOR, BLACK, 16, 0);
        return;
    }

    /* compute max value for scaling */
    float maxv = 0.0f;
    float minv = 999999.0f;
    for (uint16_t i = 0; i < sample_count; ++i) {
        float s = get_sample_at(i);
        if (s > maxv) maxv = s;
        if (s < minv) minv = s;
    }
    if (maxv <= 0.0f) maxv = 1.0f;
    
    /* Add 10% headroom for better visualization */
    float range = maxv - minv;
    if (range < 0.1f) range = 0.1f;
    maxv = maxv + range * 0.1f;
    minv = minv - range * 0.1f;
    if (minv < 0) minv = 0;

    /* draw horizontal grid lines (5 lines) with labels */
    for (int g = 0; g <= 4; ++g) {
        uint16_t yy = gy0 + (gh * g) / 4;
        lcd_DrawLine(gx0 + 1, yy, gx0 + gw, yy, GRAPH_GRID_COLOR);
        
        /* Y-axis scale labels */
        float val = maxv - (maxv - minv) * g / 4.0f;
        lcd_ShowFloatNum(g_x + 2, yy - 6, val, 3, GRAPH_TEXT_COLOR, BLACK, 12);
    }

    /* draw vertical grid lines (time markers) */
    for (int g = 1; g < 5; ++g) {
        uint16_t xx = gx0 + (gw * g) / 5;
        lcd_DrawLine(xx, gy0, xx, gy0 + gh - 1, GRAPH_GRID_COLOR);
    }

    /* draw samples as continuous line graph */
    uint16_t points = sample_count;
    if (points < 2) {
        /* single point */
        float val = get_sample_at(0);
        uint16_t px = gx0 + gw/2;
        float normalized = (val - minv) / (maxv - minv);
        if (normalized < 0) normalized = 0;
        if (normalized > 1) normalized = 1;
        uint16_t py = gy0 + gh - 1 - (uint16_t)(normalized * (gh - 1));
        lcd_DrawPoint(px, py, GRAPH_LINE_COLOR);
        lcd_DrawCircle(px, py, GRAPH_LINE_COLOR, 2, 1);
    } else {
        /* draw line graph connecting all points */
        for (uint16_t i = 1; i < points; ++i) {
            float prev = get_sample_at(i - 1);
            float cur  = get_sample_at(i);
            
            /* map time (sample index) to X coordinate */
            uint16_t x0 = gx0 + (uint32_t)((uint32_t)(i-1) * (gw-1) / (points-1));
            uint16_t x1 = gx0 + (uint32_t)((uint32_t)(i) * (gw-1) / (points-1));
            
            /* map power values to Y coordinate (inverted) */
            float norm_prev = (prev - minv) / (maxv - minv);
            float norm_cur = (cur - minv) / (maxv - minv);
            
            if (norm_prev < 0) norm_prev = 0;
            if (norm_prev > 1) norm_prev = 1;
            if (norm_cur < 0) norm_cur = 0;
            if (norm_cur > 1) norm_cur = 1;
            
            uint16_t y0 = gy0 + gh - 1 - (uint16_t)(norm_prev * (gh - 1));
            uint16_t y1 = gy0 + gh - 1 - (uint16_t)(norm_cur * (gh - 1));
            
            /* draw line segment */
            lcd_DrawLine(x0, y0, x1, y1, GRAPH_LINE_COLOR);
        }
        
        /* draw dots at data points for clarity */
        for (uint16_t i = 0; i < points; i += (points > 30 ? 5 : 1)) {
            float val = get_sample_at(i);
            uint16_t px = gx0 + (uint32_t)((uint32_t)i * (gw-1) / (points-1));
            float normalized = (val - minv) / (maxv - minv);
            if (normalized < 0) normalized = 0;
            if (normalized > 1) normalized = 1;
            uint16_t py = gy0 + gh - 1 - (uint16_t)(normalized * (gh - 1));
            lcd_DrawCircle(px, py, GRAPH_LINE_COLOR, 1, 1);
        }
    }

    /* show current (latest) value below graph */
    float latest = get_sample_at(sample_count - 1);
    lcd_ShowStr(gx0, gy0 + gh + 5, "Current:", GRAPH_VALUE_COLOR, BLACK, 12, 0);
    lcd_ShowFloatNum(gx0 + 55, gy0 + gh + 5, latest, 4, GRAPH_VALUE_COLOR, BLACK, 12);
    lcd_ShowStr(gx0 + 95, gy0 + gh + 5, "W", GRAPH_VALUE_COLOR, BLACK, 12, 0);
    
    /* show sample count and time info */
    char info[32];
    sprintf(info, "%d samples", sample_count);
    lcd_ShowStr(gx0 + gw - 70, gy0 - 12, info, GRAPH_TEXT_COLOR, BLACK, 12, 0);
}
// ...existing code...