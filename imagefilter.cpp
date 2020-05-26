#include <cmath>
#include <thread>
#include <fftw3.h>

#include "imagefilter.h"


void multiply_all(double *double_arr, const uint32_t length, const double factor) {
    const double *end = double_arr + length + 1;

    while (double_arr != end) {
        *(double_arr++) *= factor;
    }
}

void transform_and_modify(const fftw_plan &plan,
                          const fftw_plan &inverse_plan,
                          double *frequency_space,
                          double *image_space,
                          const uint32_t total_pixels,
                          const double renormalization_factor,
                          void (*frequency_modifier)(double *frequency_space,
                                                     double *image_space))
{

    // perform the FFT (image space => frequency space)
    fftw_execute(plan);

    // modify the output - fitler the top frequencies away, leaving only the specified portion of low frequencies
    frequency_modifier(frequency_space, image_space);

    // perform the IFFT (frequency space => image space)
    fftw_execute(inverse_plan);

    // FFTW provides unnormalized transforms, so IFFT(FFT(X)) = (2w) * (2h) * X
    // We only want X, so renormalize the output: multiply each element by 1/(4 * w * h).
    multiply_all(image_space, total_pixels, renormalization_factor);
}

void ImageFilter::filter_image(ImageFilter::image &img,
                               const bool self_optimize,
                               void (*frequency_modifier)(double *frequency_space,
                                                          double *image_space))
{

    // precompute image info
    const uint16_t width = img.width;
    const uint16_t height = img.height;
    const uint32_t total_pixels = width * height;

    // create an image space for each color channel
    double *image_space_r = (double *)fftw_malloc(sizeof(double) * total_pixels);

    // create a frequency space for each color channel
    double *frequency_space_r = (double *)fftw_malloc(sizeof(fftw_complex) * total_pixels);

    // define flags to control the under-the-hood behavior of FFTW
    const uint8_t transform_flags = FFTW_DESTROY_INPUT | (self_optimize ? FFTW_MEASURE : FFTW_ESTIMATE);

    // create the forward-tranform plans
    const fftw_plan plan_r = fftw_plan_r2r_2d((int)width, (int)height, image_space_r, frequency_space_r, FFTW_REDFT10, FFTW_REDFT10, transform_flags);

    // create the inverse-transform plans
    const fftw_plan plan_r_inverse = fftw_plan_r2r_2d((int)width, (int)height, frequency_space_r, image_space_r, FFTW_REDFT01, FFTW_REDFT01, transform_flags);

    // declare this variable next to where we use it, so it's near in physical memory (maybe, hopefully)
    const double renormalization_factor = 0.25f / (double)(total_pixels);

    // perform the fft in parallel on 3 seperate threads, each thread then modifies the output
    // it's structured this way so that if one thread lags behind, the others don't need to wait for it (until the very end)
    std::thread thread_r(transform_and_modify, plan_r, plan_r_inverse, frequency_space_r, image_space_r, total_pixels, renormalization_factor, frequency_modifier);

    // wait until all threads are finished
    thread_r.join();

    // reassemble the image (combine the color channels)
    //combine_image_spaces(img, total_pixels, image_space_r, image_space_g, image_space_b);
    img.image_data = image_space_r;

    // free the transform plans
    fftw_destroy_plan(plan_r);
    fftw_destroy_plan(plan_r_inverse);

    // and finally free the image and frequency spaces
    fftw_free(image_space_r);
    fftw_free(frequency_space_r);
}
