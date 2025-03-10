#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "applications.hpp"

/**
 * Brief Main function to demonstrate image compression.
 *
 * This program loads an input image in PNG format, compresses it using an SVD-based
 * method, and saves the compressed image as a PNG file.
 *
 * return 0 if the program runs successfully, -1 on failure.
 */
int main(int argc, char **argv)
{
    /**
     * DEfinition of the scale of compression
     */
    int r = (argc >= 2) ? std::stoul(argv[1]) : 20;
    int p = (argc >= 3) ? std::stoul(argv[2]) : 10;
    
    /**
     * Upload the input image in png format and
     * set the output file image
     */
    const char *filename = "../data/luna_rossa.png";
    const char *outputFilename = "../data_output/luna_rossa_out.png";

    /**
     * Create the object of the compression image application
     */
    APPLICATIONS obj;

    int Width, Height, channels;

    /**
     * Load the input image
     */
    stbi_uc *Image = stbi_load(filename, &Width, &Height, &channels, 0);

    if (Image == nullptr)
    {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return -1;
    }

    auto start = std::chrono::high_resolution_clock::now();
/**
 * Create matrices in order to compress each component, keeping the rgb colour values
 */
#ifdef RGB

#pragma omp parallel for
    for (int i = 0; i < channels; i++)
    {
        Matrix Compressed(Height, Width);
        /**
         *  Perform image compression using SVD
         */
        Compressed = obj.image_compression(Image, channels, i, Height, Width, r, p);
        /**
         * Perform backward conversion to obtain the decompressed image
         */
        obj.backward_conversion(Image, Compressed, channels, i, Height, Width);
    }
#else
    Matrix Compressed(Height, Width);
    /**
     *  Perform image compression using SVD
     */
    Compressed = obj.image_compression(Image, channels, 0, Height, Width, r, p);
    for (int i = 0; i < channels; i++)
    {
        obj.backward_conversion(Image, Compressed, channels, i, Height, Width);
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    int uncompressedSize = Height * Width;
    int compressedSize = Height * r + r + r * Width;

    double compressionRatio = uncompressedSize / compressedSize;

    std::cout << "Uncompressed size: " << uncompressedSize << " bytes" << std::endl;
    std::cout << "Compressed size: " << compressedSize << " bytes" << std::endl;
    std::cout << "Compression ratio: " << compressionRatio << std::endl;

    std::cout << "Duration: " << duration.count() << " s" << std::endl;

    /**
     * Save the decompressed image
     */
    stbi_write_png(outputFilename, Width, Height, channels, Image, Width * channels);

    /**
     * Free allocated memory
     */
    stbi_image_free(Image);

    return 0;
}