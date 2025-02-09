#ifndef _HEADER_GUARD_Spherical_Geometry_Clustering_PolarCoordinates_
#define _HEADER_GUARD_Spherical_Geometry_Clustering_PolarCoordinates_

/////////////////////
// Headers BEGIN:{ //
/////////////////////
#include "../StaticArray.hh"
///////////////////
// Headers END.} //
///////////////////

/////////////////////////////
// GPS Coordinates BEGIN:{ //
/////////////////////////////
#define _MACRO_CLASS_NAME_ D2GPS_Coordinates
template <typename TF = double, typename TI = long> class _MACRO_CLASS_NAME_ {
    ////////////////////////
    // Definitions BEGIN: //
    ////////////////////////
  public:
    using TYPE_FLOAT     = TF;
    using TYPE_INT       = TI;
    using TYPE_SELF      = _MACRO_CLASS_NAME_<TYPE_FLOAT, TYPE_INT>;
    using TYPE_PAIR      = StaticArray::ND_ARRAY<2, TYPE_FLOAT>;
    using TYPE_CARTITION = StaticArray::ND_ARRAY<3, TYPE_FLOAT>;
    using TYPE_UINT      = CPPFileIO::TYPE_U32;
    using TYPE_ULONG     = CPPFileIO::TYPE_U64;
    using TYPE_BYTE      = CPPFileIO::TYPE_BYTE;

    inline static TYPE_FLOAT constexpr PI() { return M_PI; }
    inline static TYPE_FLOAT constexpr PIB2() { return M_PI / 2.0; }
    inline static TYPE_FLOAT constexpr EarthRadius() { return 6367000; }
    //////////////////////
    // Definitions END. //
    //////////////////////

    //////////////////////////
    // Data Elements BEGIN: //
    //////////////////////////
  public:
    TYPE_FLOAT latitude, longitude;
    ////////////////////////
    // Data Elements END. //
    ////////////////////////

    ////////////////////////////////
    // Dealing with pairs BEGIN:{ //
    ////////////////////////////////
  public:
    inline TYPE_PAIR &AS_PAIR() {
        return reinterpret_cast<TYPE_PAIR &>(this[0]);
    }

    inline TYPE_PAIR const &AS_PAIR() const {
        return reinterpret_cast<TYPE_PAIR const &>(this[0]);
    }

    inline void operator=(TYPE_PAIR const &in) { AS_PAIR() = in; }
    //////////////////////////////
    // Dealing with pairs END.} //
    //////////////////////////////

    /////////////////////////////////////////
    // Spherical polar coordinates BEGIN:{ //
    /////////////////////////////////////////
  public:
    inline TYPE_FLOAT THETA() const {
        return (M_PI / 2.0) - (latitude * M_PI / 180.0);
    }

    inline TYPE_FLOAT PHI() const { return (longitude * M_PI / 180.0); }

    inline TYPE_FLOAT SIN_THETA() const { return std::sin(THETA()); }
    inline TYPE_FLOAT COS_THETA() const { return std::cos(THETA()); }
    inline TYPE_FLOAT SIN_PHI() const { return std::sin(PHI()); }
    inline TYPE_FLOAT COS_PHI() const { return std::cos(PHI()); }

    inline void SET_THETA(TYPE_FLOAT const in) {
        if ((0 <= in) && (in <= M_PI)) {
            latitude = 90.0 - (in * 180.0 / M_PI);
        } else {
            latitude = -9999.0;
        }
    }

    inline void SET_PHI(TYPE_FLOAT const in) {
        if ((-M_PI <= in) && (in <= M_PI)) {
            longitude = (in * 180.0 / M_PI);
        } else {
            longitude = -9999.0;
        }
    }
    ///////////////////////////////////////
    // Spherical polar coordinates END.} //
    ///////////////////////////////////////

    //////////////////////////////////////////
    // Carition coordinates related BEGIN:{ //
    //////////////////////////////////////////
  public:
    inline TYPE_FLOAT X() const { return SIN_THETA() * COS_PHI(); }
    inline TYPE_FLOAT Y() const { return SIN_THETA() * SIN_PHI(); }
    inline TYPE_FLOAT Z() const { return COS_THETA(); }

    inline TYPE_CARTITION GET_CARTITION() const {
        if (!IS_VALID()) {
            TYPE_CARTITION ret;
            ret[0] = -9999;
            ret[1] = -9999;
            ret[2] = -9999;
            return ret;
        }
        TYPE_FLOAT const ct1 = COS_THETA();
        TYPE_FLOAT const st1 = SIN_THETA();
        TYPE_FLOAT const cp1 = COS_PHI();
        TYPE_FLOAT const sp1 = SIN_PHI();
        TYPE_FLOAT const x1  = st1 * cp1;
        TYPE_FLOAT const y1  = st1 * sp1;
        TYPE_FLOAT const z1  = ct1;
        TYPE_CARTITION   ret;
        ret[0] = x1;
        ret[1] = y1;
        ret[2] = z1;
        return ret;
    }

    inline void FROM_CARTITION(TYPE_CARTITION const &in) {

        TYPE_FLOAT const RT2 = std::pow(in[0], 2) + std::pow(in[1], 2);
        TYPE_FLOAT const RT  = std::sqrt(RT2);

        TYPE_FLOAT const R2 = RT2 + std::pow(in[2], 2);
        TYPE_FLOAT const R  = std::sqrt(R2);

        TYPE_FLOAT theta = -9999;
        if (R != 0) { theta = std::acos(in[2] / R); }
        SET_THETA(theta);

        TYPE_FLOAT phi = -9999;
        if (RT != 0) {
            phi = std::acos(in[0] / RT);
            if (in[1] < 0) { phi = -phi; }
        }
        SET_PHI(phi);
    }
    ////////////////////////////////////////
    // Carition coordinates related END.} //
    ////////////////////////////////////////

    ////////////////////////////
    // Main Functions BEGIN:{ //
    ////////////////////////////
  public:
    inline TYPE_FLOAT HaverSineDistance(TYPE_SELF const &other) const {
        if (!IS_VALID()) { return -9999.0; }

        TYPE_FLOAT constexpr r = EarthRadius();

        TYPE_FLOAT const ct1 = COS_THETA();
        TYPE_FLOAT const st1 = SIN_THETA();
        TYPE_FLOAT const cp1 = COS_PHI();
        TYPE_FLOAT const sp1 = SIN_PHI();

        TYPE_FLOAT const ct2 = other.COS_THETA();
        TYPE_FLOAT const st2 = other.SIN_THETA();
        TYPE_FLOAT const cp2 = other.COS_PHI();
        TYPE_FLOAT const sp2 = other.SIN_PHI();

        TYPE_FLOAT const x1 = st1 * cp1;
        TYPE_FLOAT const y1 = st1 * sp1;
        TYPE_FLOAT const z1 = ct1;

        TYPE_FLOAT const x2 = st2 * cp2;
        TYPE_FLOAT const y2 = st2 * sp2;
        TYPE_FLOAT const z2 = ct2;

        TYPE_FLOAT ca = (x1 * x2) + (y1 * y2) + (z1 * z2);
        if (ca > 1) {
            ca = 1;
        } else if (ca < -1) {
            ca = -1;
        }
        TYPE_FLOAT const a = std::acos(ca);

        return CPPFileIO::mymod(r * a);
    }

    inline bool IS_VALID() const {
        return (!std::isnan(latitude)) && (!std::isnan(longitude)) &&
               ((-90.0 <= latitude) && (latitude <= 90.0)) &&
               ((-180.0 <= longitude) && (longitude <= 180.0));
    }

    inline TYPE_UINT uint_longitude() const {
        CPPFileIO::BinMapper<TYPE_INT, TYPE_FLOAT> mapper;
        mapper(CPPFileIO::MAX_U32(), -180.0, 180.0);
        TYPE_UINT const ret = mapper(longitude);
        return ret;
    }

    inline TYPE_UINT uint_latitude() const {
        CPPFileIO::BinMapper<TYPE_INT, TYPE_FLOAT> mapper;
        mapper(CPPFileIO::MAX_U32(), -90.0, 90.0);
        TYPE_UINT const ret = mapper(latitude);
        return ret;
    }

    inline TYPE_ULONG z_curve() const {
        return CPPFileIO::get_z_curve(uint_latitude(), uint_longitude());
    }
    //////////////////////////
    // Main Functions END.} //
    //////////////////////////

    ////////////////////////
    // Convinence BEGIN:{ //
    ////////////////////////
  public:
    inline TYPE_FLOAT operator()(TYPE_SELF const &in) const {
        return this->HaverSineDistance(in);
    }

    inline TYPE_ULONG operator()() const { return z_curve(); }

    inline TYPE_FLOAT magnitude_2() const {
        return std::pow(latitude, 2) + std::pow(longitude, 2);
    }

    inline TYPE_FLOAT magnitude() const { return std::sqrt(magnitude_2()); }

    inline TYPE_SELF unit_vector() const {
        TYPE_FLOAT const mag = magnitude();
        TYPE_SELF const  ret = {latitude / mag, longitude / mag};
        return ret;
    }

    inline TYPE_SELF rotate(TYPE_FLOAT const angle_degrees) const {
        TYPE_FLOAT const angle_radians = angle_degrees * M_PI / 180.0;
        TYPE_FLOAT const cos_theta     = std::cos(angle_radians);
        TYPE_FLOAT const sin_theta     = std::sin(angle_radians);
        TYPE_SELF const ret = {(latitude * cos_theta) - (longitude * sin_theta),
                               (latitude * sin_theta) +
                                 (longitude * cos_theta)};
        return ret;
    }
    //////////////////////
    // Convinence END.} //
    //////////////////////

    //////////////////////////////
    // Vector operators BEGIN:{ //
    //////////////////////////////
  public:
    inline TYPE_SELF operator+(TYPE_SELF const &in) const {
        TYPE_SELF const ret = {latitude + in.latitude,
                               longitude + in.longitude};
        return ret;
    }

    inline TYPE_SELF operator-(TYPE_SELF const &in) const {
        TYPE_SELF const ret = {latitude - in.latitude,
                               longitude - in.longitude};
        return ret;
    }

    inline TYPE_SELF operator*(TYPE_FLOAT const in) const {
        TYPE_SELF const ret = {latitude * in, longitude * in};
        return ret;
    }

    inline TYPE_FLOAT operator*(TYPE_SELF const in) const {
        TYPE_FLOAT const ret =
          (latitude * in.latitude) + (longitude * in.longitude);
        return ret;
    }
    ////////////////////////////
    // Vector operators END.} //
    ////////////////////////////

  public:
    _MACRO_CLASS_NAME_(TYPE_FLOAT const inlat, TYPE_FLOAT const inlon)
      : latitude(inlat), longitude(inlon) {}

    _MACRO_CLASS_NAME_(TYPE_PAIR const &in) { this[0] = in; }
    _MACRO_CLASS_NAME_(TYPE_SELF const &in) { this[0] = in; }

    _MACRO_CLASS_NAME_() {}
    ~_MACRO_CLASS_NAME_() {}
};
#undef _MACRO_CLASS_NAME_
///////////////////////////
// GPS Coordinates END.} //
///////////////////////////

#endif
