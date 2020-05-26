#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H

#include <cstdint>

namespace ImageFilter
{
    struct image {
        const uint16_t width;
        const uint16_t height;
        const double *image_data;

        image(double *img, uint16_t w, uint16_t h) : width(w), height(h), image_data(img) {}
    };

    void filter_image(image &img,
                          const bool self_optimize,
                          void (*frequency_modifier)(double *frequency_space,
                                                     double *image_space));
};

#endif // IMAGEFILTER_H
