#include <stdint.h>
#include <SDL.h>
#include <math.h>
#include <stdbool.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_WIDTH_HALF SCREEN_WIDTH/2
#define SCREEN_HEIGHT_HALF SCREEN_HEIGHT/2

SDL_Surface* screen;

// Structure to represent a 3D point
typedef struct {
    int x, y, z;
} Point3D;

// Structure to represent a 2D point
typedef struct {
    int x, y;
} Point2D;

#define FIXED_POINT_SHIFT 8
#define FIXED_POINT_SCALE (1 << FIXED_POINT_SHIFT)

int sin_lookup[360] = {
0,4,8,13,17,22,26,31,35,40,44,48,53,57,61,66,70,74,79,83,87,91,95,100,104,108,112,116,120,124,128,131,135,139,143,146,150,154,157,161,164,167,171,174,177,181,184,187,190,193,196,198,201,204,207,209,212,214,217,219,221,223,226,228,230,232,233,235,237,238,240,242,243,244,246,247,248,249,250,251,252,252,253,254,254,255,255,255,255,255,255,255,255,255,255,255,254,254,253,252,252,251,250,249,248,247,246,244,243,242,240,238,237,235,233,232,230,228,226,223,221,219,217,214,212,209,207,204,201,198,196,193,190,187,184,181,177,174,171,167,164,161,157,154,150,146,143,139,135,131,128,124,120,116,112,108,104,100,95,91,87,83,79,74,70,66,61,57,53,48,44,40,35,31,26,22,17,13,8,4,0,-4,-8,-13,-17,-22,-26,-31,-35,-40,-44,-48,-53,-57,-61,-66,-70,-74,-79,-83,-87,-91,-95,-100,-104,-108,-112,-116,-120,-124,-127,-131,-135,-139,-143,-146,-150,-154,-157,-161,-164,-167,-171,-174,-177,-181,-184,-187,-190,-193,-196,-198,-201,-204,-207,-209,-212,-214,-217,-219,-221,-223,-226,-228,-230,-232,-233,-235,-237,-238,-240,-242,-243,-244,-246,-247,-248,-249,-250,-251,-252,-252,-253,-254,-254,-255,-255,-255,-255,-255,-255,-255,-255,-255,-255,-255,-254,-254,-253,-252,-252,-251,-250,-249,-248,-247,-246,-244,-243,-242,-240,-238,-237,-235,-233,-232,-230,-228,-226,-223,-221,-219,-217,-214,-212,-209,-207,-204,-201,-198,-196,-193,-190,-187,-184,-181,-177,-174,-171,-167,-164,-161,-157,-154,-150,-146,-143,-139,-135,-131,-128,-124,-120,-116,-112,-108,-104,-100,-95,-91,-87,-83,-79,-74,-70,-66,-61,-57,-53,-48,-44,-40,-35,-31,-26,-22,-17,-13,-8,-4};
int cos_lookup[360] = {
256,255,255,255,255,255,254,254,253,252,252,251,250,249,248,247,246,244,243,242,240,238,237,235,233,232,230,228,226,223,221,219,217,214,212,209,207,204,201,198,196,193,190,187,184,181,177,174,171,167,164,161,157,154,150,146,143,139,135,131,127,124,120,116,112,108,104,100,95,91,87,83,79,74,70,66,61,57,53,48,44,40,35,31,26,22,17,13,8,4,0,-4,-8,-13,-17,-22,-26,-31,-35,-40,-44,-48,-53,-57,-61,-66,-70,-74,-79,-83,-87,-91,-95,-100,-104,-108,-112,-116,-120,-124,-128,-131,-135,-139,-143,-146,-150,-154,-157,-161,-164,-167,-171,-174,-177,-181,-184,-187,-190,-193,-196,-198,-201,-204,-207,-209,-212,-214,-217,-219,-221,-223,-226,-228,-230,-232,-233,-235,-237,-238,-240,-242,-243,-244,-246,-247,-248,-249,-250,-251,-252,-252,-253,-254,-254,-255,-255,-255,-255,-255,-255,-255,-255,-255,-255,-255,-254,-254,-253,-252,-252,-251,-250,-249,-248,-247,-246,-244,-243,-242,-240,-238,-237,-235,-233,-232,-230,-228,-226,-223,-221,-219,-217,-214,-212,-209,-207,-204,-201,-198,-196,-193,-190,-187,-184,-181,-177,-174,-171,-167,-164,-161,-157,-154,-150,-146,-143,-139,-135,-131,-127,-124,-120,-116,-112,-108,-104,-100,-95,-91,-87,-83,-79,-74,-70,-66,-61,-57,-53,-48,-44,-40,-35,-31,-26,-22,-17,-13,-8,-4,0,4,8,13,17,22,26,31,35,40,44,48,53,57,61,66,70,74,79,83,87,91,95,100,104,108,112,116,120,124,127,131,135,139,143,146,150,154,157,161,164,167,171,174,177,181,184,187,190,193,196,198,201,204,207,209,212,214,217,219,221,223,226,228,230,232,233,235,237,238,240,242,243,244,246,247,248,249,250,251,252,252,253,254,254,255,255,255,255,255};

// Define a 3D cube
Point3D cube[] = {
    { -100,  100,  100 },
    {  100,  100,  100 },
    {  100, -100,  100 },
    { -100, -100,  100 },
    { -100,  100, -100 },
    {  100,  100, -100 },
    {  100, -100, -100 },
    { -100, -100, -100 },
};

// Define the edges of the cube
int edges[][2] = {
    { 0, 1 },
    { 1, 2 },
    { 2, 3 },
    { 3, 0 },
    { 4, 5 },
    { 5, 6 },
    { 6, 7 },
    { 7, 4 },
    { 0, 4 },
    { 1, 5 },
    { 2, 6 },
    { 3, 7 },
};

Point3D rotateX(Point3D p, int angle) {
    // Ensure the angle is within 0-359 degrees
    angle = angle % 360;
    
    int sinA = sin_lookup[angle];
    int cosA = cos_lookup[angle];

    Point3D result;
    result.x = p.x;
    result.y = (p.y * cosA - p.z * sinA) >> FIXED_POINT_SHIFT;
    result.z = (p.y * sinA + p.z * cosA) >> FIXED_POINT_SHIFT;
    return result;
}

Point3D rotateY(Point3D p, int angle) {
    // Ensure the angle is within 0-359 degrees
    angle = angle % 360;
    
    int sinA = sin_lookup[angle];
    int cosA = cos_lookup[angle];

    Point3D result;
    result.x = (p.x * cosA + p.z * sinA) >> FIXED_POINT_SHIFT;
    result.y = p.y;
    result.z = (p.z * cosA - p.x * sinA) >> FIXED_POINT_SHIFT;
    return result;
}

// Function to project a 3D point onto a 2D surface
Point2D project(Point3D p, int distance) {
    Point2D result;
    result.x = p.x * distance / (distance - p.z);
    result.y = p.y * distance / (distance - p.z);
    return result;
}

void setPixel(uint_fast32_t x, uint_fast32_t y, Uint32 pixel)
{
	if (x > screen->w || y > screen->w)
	return;
	
	uint8_t *p = (uint8_t *)screen->pixels + y * screen->pitch + x * 2;
	*(uint16_t *)p = pixel;
}

void line(int x1, int y1, int x2, int y2, int color) 
{
    int ix;
    int iy;
 
    unsigned short delta_x = (x2 > x1?(ix = 1, x2 - x1):(ix = -1, x1 - x2)) << 1;
    unsigned short delta_y = (y2 > y1?(iy = 1, y2 - y1):(iy = -1, y1 - y2)) << 1;
 
	setPixel(x1,y1,color);
    if (delta_x >= delta_y) {
        int error = delta_y - (delta_x >> 1);
        while (x1 != x2) {
            if (error >= 0) {
                if (error || (ix > 0)) {
                    y1 += iy;
                    error -= delta_x;
                }
         }
            x1 += ix;
            error += delta_y;
            setPixel(x1,y1,color);
        }
    } else {
        int error = delta_x - (delta_y >> 1);
        while (y1 != y2) {
            if (error >= 0) {
                if (error || (iy > 0)) {
                    x1 += ix;
                    error -= delta_y;
                }
            }
            y1 += iy;
            error += delta_x;  
            setPixel(x1,y1, color);
        }
    }
}


#if 0
// How the Sin/Cos tables were computed
void init_trig_tables() {
    for (int i = 0; i < 360; i++) {
        float rad = i * M_PI / 180.0;
        sin_lookup[i] = (int)(sin(rad) * FIXED_POINT_SCALE);
        cos_lookup[i] = (int)(cos(rad) * FIXED_POINT_SCALE);
    }
}
#endif

int main(int argc, char** argv) {
	
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    // Create a new window
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_DOUBLEBUF);
    if (!screen) {
        return 1;
    }

    // Main loop
    int angle = 0;
    while (1) 
    {
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return 0;
            }
        }

        // Clear the screen
        SDL_FillRect(screen, NULL, 0);

        // Draw the cube
        for (int i = 0; i < 12; i++) {
            Point3D p1 = rotateX(cube[edges[i][0]], angle);
            p1 = rotateY(p1, angle);
            Point3D p2 = rotateX(cube[edges[i][1]], angle);
            p2 = rotateY(p2, angle);
            Point2D s1 = project(p1, 256);
            Point2D s2 = project(p2, 256);
            s1.x += SCREEN_WIDTH_HALF;
            s1.y += SCREEN_HEIGHT_HALF;
            s2.x += SCREEN_WIDTH_HALF;
            s2.y += SCREEN_HEIGHT_HALF;
            line(s1.x, s1.y, s2.x, s2.y, 0xFFFF);
        }

        // Update the screen
        SDL_Flip(screen);

        // Increase the angle
        angle = (angle + 1) % 360;

        // Cap the frame rate
        SDL_Delay(16);
    }

    return 1;
}



