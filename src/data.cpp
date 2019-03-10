#include "data.hpp"

#include <array>
#include <string>

#include "logger.hpp"
#include "url.hpp"

static std::array<std::string, 49> sources_ = {
    {"https://celestrak.com/NORAD/elements/tle-new.txt",
     "https://celestrak.com/NORAD/elements/stations.txt",
     "https://celestrak.com/NORAD/elements/visual.txt",
     "https://celestrak.com/NORAD/elements/active.txt",
     "https://celestrak.com/NORAD/elements/analyst.txt",
     "https://celestrak.com/NORAD/elements/1999-025.txt",
     "https://celestrak.com/NORAD/elements/iridium-33-debris.txt",
     "https://celestrak.com/NORAD/elements/cosmos-2251-debris.txt",
     "https://celestrak.com/NORAD/elements/2012-044.txt",
     "https://celestrak.com/NORAD/elements/weather.txt",
     "https://celestrak.com/NORAD/elements/noaa.txt",
     "https://celestrak.com/NORAD/elements/goes.txt",
     "https://celestrak.com/NORAD/elements/resource.txt",
     "https://celestrak.com/NORAD/elements/sarsat.txt",
     "https://celestrak.com/NORAD/elements/dmc.txt",
     "https://celestrak.com/NORAD/elements/tdrss.txt",
     "https://celestrak.com/NORAD/elements/argos.txt",
     "https://celestrak.com/NORAD/elements/planet.txt",
     "https://celestrak.com/NORAD/elements/spire.txt",
     "https://celestrak.com/NORAD/elements/geo.txt",
     "https://celestrak.com/NORAD/elements/intelsat.txt",
     "https://celestrak.com/NORAD/elements/ses.txt",
     "https://celestrak.com/NORAD/elements/iridium.txt",
     "https://celestrak.com/NORAD/elements/iridium-NEXT.txt",
     "https://celestrak.com/NORAD/elements/orbcomm.txt",
     "https://celestrak.com/NORAD/elements/globalstar.txt",
     "https://celestrak.com/NORAD/elements/amateur.txt",
     "https://celestrak.com/NORAD/elements/x-comm.txt",
     "https://celestrak.com/NORAD/elements/other-comm.txt",
     "https://celestrak.com/NORAD/elements/satnogs.txt",
     "https://celestrak.com/NORAD/elements/gorizont.txt",
     "https://celestrak.com/NORAD/elements/raduga.txt",
     "https://celestrak.com/NORAD/elements/molniya.txt",
     "https://celestrak.com/NORAD/elements/gps-ops.txt",
     "https://celestrak.com/NORAD/elements/glo-ops.txt",
     "https://celestrak.com/NORAD/elements/galileo.txt",
     "https://celestrak.com/NORAD/elements/beidou.txt",
     "https://celestrak.com/NORAD/elements/sbas.txt",
     "https://celestrak.com/NORAD/elements/nnss.txt",
     "https://celestrak.com/NORAD/elements/musson.txt",
     "https://celestrak.com/NORAD/elements/science.txt",
     "https://celestrak.com/NORAD/elements/geodetic.txt",
     "https://celestrak.com/NORAD/elements/engineering.txt",
     "https://celestrak.com/NORAD/elements/education.txt",
     "https://celestrak.com/NORAD/elements/military.txt",
     "https://celestrak.com/NORAD/elements/radar.txt",
     "https://celestrak.com/NORAD/elements/cubesat.txt",
     "https://celestrak.com/NORAD/elements/other.txt",
     "https://ssd.jpl.nasa.gov/txt/p_elem_t2.txt"}};

void astri::data::validate_data() {
  for (std::size_t i = 0; i < sources_.size(); ++i) {
    std::string file_name =
        "./data" + sources_[i].substr(sources_[i].find_last_of('/'));
    url::retrieve_once(file_name, sources_[i]);
  }
}
