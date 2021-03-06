#include <iostream>
#include "cgutilities.h"
#include "rgbimage.h"
#include "simpleraytracer.h"


int main(int argc, const char * argv[]) {
    unsigned int detail = 20;
    bool debug = false;
    if (detail > 1) {
        debug = true;
    }
    Scene scene(detail);
    RGBImage image(640,480);
    SimpleRayTracer tracer(3);
    tracer.traceScene(scene, image, debug);
    image.saveToDisk("raytracing_image.bmp");
    return 0;
}
