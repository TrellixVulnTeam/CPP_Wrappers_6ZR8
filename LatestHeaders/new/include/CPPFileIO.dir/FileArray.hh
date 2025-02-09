﻿#ifndef _HEADER_GUARD_CPPFileIO_FileArray_
#define _HEADER_GUARD_CPPFileIO_FileArray_

////////////////////
// Headers BEGIN: //
////////////////////
#include "../Headers.hh"
#include "./Basic.hh"
#include "./FileFD.hh"
#include "./D1.hh"
//////////////////
// Headers END. //
//////////////////

#define _MACRO_CLASS_NAME_ FileArray
template <typename T> class _MACRO_CLASS_NAME_ {
    ////////////////////////
    // Definitions BEGIN: //
    ////////////////////////
  public:
    using TYPE_SELF    = _MACRO_CLASS_NAME_;
    using TYPE_ELEMENT = T;

    static inline size_t constexpr Sizes(size_t const in) {
        size_t constexpr sizes[4] = {4096, sizeof(TYPE_ELEMENT),
                                     LCM(sizes[0], sizes[1]),
                                     sizes[2] / sizes[1]}; //
        return sizes[in];
    }
    //////////////////////
    // Definitions END. //
    //////////////////////

    //////////////////////////
    // Data Elements BEGIN: //
    //////////////////////////
  private:
    TYPE_ELEMENT *mainptr;
    std::string   filename;
    FileFD        filefd;
    size_t        offset;
    size_t        begin, end, length;
    size_t        act_begin, act_end, act_length;
    ////////////////////////
    // Data Elements END. //
    ////////////////////////

    //////////////////////
    // File Mode BEGIN: //
    //////////////////////
  public:
    inline void Reset() {
        begin      = 0;
        act_begin  = 0;
        end        = 0;
        act_end    = 0;
        length     = 0;
        act_length = 0;
    }

    inline void construct(std::string const Afilename = std::string("outfile"),
                          size_t const      Aoffset   = 0) {

        filename = Afilename;
        offset   = Aoffset;
        Reset();
        filefd(filename).readfile();
    }

    inline void destroy() {
        Reset();
        filefd.destroy();
    }

    inline void
    reconstruct(std::string const Afilename = std::string("outfile"),
                size_t const      Aoffset   = 0) {

        destroy();
        construct(Afilename, Aoffset);
    }

    inline void writeable(bool const arg = true) {
        if (arg) {
            Reset();
            filefd(filename).appendfile();
        } else {
            Reset();
            filefd(filename).readfile();
        }
    }
    ////////////////////
    // File Mode END. //
    ////////////////////

    //////////////////////////////////
    // Main Mapping Function BEGIN: //
    //////////////////////////////////
  public:
    inline void map(size_t const t_begin = 0, size_t const t_length = 1) {
        size_t const t_end = t_begin + t_length;
        if ((t_begin < begin) || (t_end > end)) {
            /* Match to sector sizes: */ {
                begin = static_cast<size_t>(static_cast<double>(t_begin) /
                                            static_cast<double>(Sizes(3))) *
                        Sizes(3);

                end = static_cast<size_t>(
                        1 + (static_cast<double>(t_begin + t_length) /
                             static_cast<double>(Sizes(3)))) *
                      Sizes(3);

                length = end - begin;
            }
            /* Reinitiate map: */ {
                filefd.unmapfile();
                mainptr = reinterpret_cast<TYPE_ELEMENT *>(
                  filefd.mapfile((length * Sizes(1)),
                                 (begin * Sizes(1)) + (offset * Sizes(0))));
            }
        }
    }
    ////////////////////////////////
    // Main Mapping Function END. //
    ////////////////////////////////

    ///////////////////////
    // File Sizes BEGIN: //
    ///////////////////////
  public:
    inline off_t filesize() { return filefd.sizefile(); }

    inline off_t size() {
        return filefd.sizefile() / Sizes(1); //
    }

    inline off_t size(long const num) {
        filefd.unmapfile();
        filefd.truncatefile(num * Sizes(1));
        return size();
    }
    /////////////////////
    // File Sizes END. //
    /////////////////////

    ///////////////////////
    // Convinence BEGIN: //
    ///////////////////////
  public:
    inline TYPE_ELEMENT &operator()(size_t const A_begin,
                                    size_t const A_length = 1) {
        map(A_begin, A_length);
        TYPE_ELEMENT &ret = mainptr[A_begin - begin];
        return ret;
    }

    template <typename TTI>
    inline void operator()(size_t const                       A_begin,
                           Dynamic1DArray<TYPE_ELEMENT, TTI> &in) {
        TTI        A_length            = in();
        auto const filesize            = size();
        TTI const  A_end               = mymin(A_begin + A_length, filesize);
        A_length                       = A_end - A_begin;
        TYPE_ELEMENT *const       dest = in.GET_DATA();
        TYPE_ELEMENT const *const src  = &(this[0](A_begin, A_length));
        memcpy(dest, src, A_length * sizeof(TYPE_ELEMENT));
    }

    inline off_t operator()() { return size(); }

    inline FileArray &operator()(std::string const Afilename,
                                 size_t const      Aoffset = 0) {
        reconstruct(Afilename, Aoffset);
        return (*this);
    }
    /////////////////////
    // Convinence END. //
    /////////////////////

    /////////////////////////////////////
    // Constructor & Destructor BEGIN: //
    /////////////////////////////////////
  public:
    _MACRO_CLASS_NAME_(std::string const Afilename = std::string("outfile"),
                       size_t const      Aoffset   = 0) {
        construct(Afilename, Aoffset);
    }

    ~_MACRO_CLASS_NAME_() { destroy(); }
    ///////////////////////////////////
    // Constructor & Destructor END. //
    ///////////////////////////////////
};
#undef _MACRO_CLASS_NAME_

#endif
