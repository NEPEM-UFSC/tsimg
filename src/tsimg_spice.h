#pragma once

// Aggregator header for backward compatibility
// This file includes all the new modular headers that replaced the monolithic tsimg_spice.h

#include "utils/Logger.h"
#include "utils/FileHandler.h"
#include "utils/ImageValidator.h"
#include "utils/Base64.h"
#include "utils/FileIO.h"
#include "utils/JsonUtils.h"
#include "utils/ImageProcessor.h"

#include "spice/Image.h"
#include "spice/SpiceContent.h"
#include "spice/HTMLBuilder.h"
#include "spice/TemplateWriter.h"
#include "spice/SPICEBuilder.h"
#include "spice/SPICE.h"
