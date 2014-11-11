// Port of the gaussian blur algorithm by Ivan Kuckir,
// released inther MIT licence. For details, see
// http://blog.ivank.net/fastest-gaussian-blur.html

#ifndef GaussianBlur_H
#define GaussianBlur_H

#include <glm/glm.hpp>


void GaussBlur(glm::vec4* scl, glm::vec4* tcl, int w, int h, int r);


#endif
