#pragma once
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <stack>
#include <map>
#include <iostream>
#include <fstream>
#include <functional>
#include <exception>
#include <algorithm>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/noise.hpp>
#include <glm/gtx/random.hpp>
using namespace glm;

#include "aux_math.h"

#define proprw(t, n, gc) inline t& n() gc
#define propr(t, n, gc) inline t n() const gc
