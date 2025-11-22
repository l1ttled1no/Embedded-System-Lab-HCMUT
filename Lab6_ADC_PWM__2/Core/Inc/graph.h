// ...existing code...
#ifndef GRAPH_H
#define GRAPH_H

#include "lcd.h"
// #include "sensor.h"
#include "global.h"

/* sample period in seconds (graph update interval) */
#define DATA_FETCH_FREQ_S 10

/* number of samples to keep / display */
#define GRAPH_MAX_SAMPLES 120

/* Initialize graph subsystem with default position and size
 * Call this from main() after lcd_init()
 */
void graph_init(void);

/* clear internal buffer and LCD area */
void graph_clear(void);

/* read sensors and push a new sample (call every DATA_FETCH_FREQ_S seconds) */
void graph_add_sample(void);

/* force redraw of the graph area (call after graph_clear() or externally) */
void graph_render(void);

#endif // GRAPH_H
