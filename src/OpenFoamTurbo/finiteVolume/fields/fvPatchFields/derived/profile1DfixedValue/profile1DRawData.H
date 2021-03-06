/*---------------------------------------------------------------------------*\
 =========                   |
 \\      /   F ield          | OpenFOAM: The Open Source CFD Toolbox
  \\    /    O peration      |
   \\  /     A nd            | Copyright Hydro-Quebec - IREQ, 2008
    \\/      M anipulation   |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
    profile1DRawData

Description
    Read an ASCII file containing a 1D radial profile

    File format supported:
            turboCSV: the same kind as the CSV format generated by CFX-5
	    Other file format could be added eventually

    See the file profile1DRawData.C for a complete description of the turboCSV
    file format

    See the file profileExample.csv for an example of the turboCSV file format.



Authors 
    Martin Beaudoin, Hydro-Quebec - IREQ
    Maryse Page,     Hydro-Quebec - IREQ
    Robert Magnan,   Hydro-Quebec - IREQ

\*---------------------------------------------------------------------------*/

#ifndef profile1DRawData_H
#define profile1DRawData_H

#include <stdlib.h>
#include <list>
#include <string>
#include <map>
#include <utility>  // for std::pair
#include <vector>
#include <algorithm>

namespace Foam
{

// Utility class: STL version of tolower
class to_lower
{
public:
    char operator() (char c) const        // notice the return type
        {
            return tolower(c);
        }
};

inline std::string toLower (const std::string & str)
{
    std::string lcase = str;
    std::transform (str.begin(), str.end(), lcase.begin(), to_lower());

    return lcase;
}


// Specify field types

#define _VELOCITY_STR_TOKEN   "Velocity"    // vectorial field
#define _VELOCITY_X_STR_TOKEN "Velocity_X"  // scalar field
#define _VELOCITY_Y_STR_TOKEN "Velocity_Y"  // scalar field
#define _VELOCITY_Z_STR_TOKEN "Velocity_Z"  // scalar field
#define _PRESSURE_STR_TOKEN   "Pressure"    // scalar field
#define _K_STR_TOKEN          "K"           // scalar field
#define _EPSILON_STR_TOKEN    "Epsilon"     // scalar field
#define _OMEGA_STR_TOKEN      "Omega"       // scalar field

#define _RADIAL_STR_TOKEN     "R"           // radial profile
#define _MERIDIAN_STR_TOKEN   "Z"           // meridian profile

// Conversion string to enum for each field types
enum profile1DField {BAD_FIELD=0, VELOCITY, VELOCITY_X, VELOCITY_Y, VELOCITY_Z, PRESSURE, K, EPSILON, OMEGA};
typedef std::map<const std::string, const profile1DField> m_profile1DFieldXlate_from_string;

const m_profile1DFieldXlate_from_string::value_type profile1DFieldXlate_map_init[] = {
    m_profile1DFieldXlate_from_string::value_type( toLower(_VELOCITY_STR_TOKEN),   VELOCITY),
    m_profile1DFieldXlate_from_string::value_type( toLower(_VELOCITY_X_STR_TOKEN), VELOCITY_X),
    m_profile1DFieldXlate_from_string::value_type( toLower(_VELOCITY_Y_STR_TOKEN), VELOCITY_Y),
    m_profile1DFieldXlate_from_string::value_type( toLower(_VELOCITY_Z_STR_TOKEN), VELOCITY_Z),
    m_profile1DFieldXlate_from_string::value_type( toLower(_PRESSURE_STR_TOKEN),   PRESSURE),
    m_profile1DFieldXlate_from_string::value_type( toLower(_K_STR_TOKEN),          K),
    m_profile1DFieldXlate_from_string::value_type( toLower(_EPSILON_STR_TOKEN),    EPSILON),
    m_profile1DFieldXlate_from_string::value_type( toLower(_OMEGA_STR_TOKEN),    OMEGA)
};

// Conversion string to enum for each profile types
enum profile1DType {BAD_PROFILE=0, R, Z};
typedef std::map<const std::string, const profile1DType> m_profile1DTypeXlate_from_string;

const m_profile1DTypeXlate_from_string::value_type profile1DTypeXlate_map_init[] = {
    m_profile1DTypeXlate_from_string::value_type( toLower(_RADIAL_STR_TOKEN),    R),
    m_profile1DTypeXlate_from_string::value_type( toLower(_MERIDIAN_STR_TOKEN),  Z)
};

//- Declaration of some utility functions.
template<class TypeEnum, class TypeContainerXlate>
const TypeEnum       string_to_turboCSVEnum  (std::string s_field, TypeContainerXlate mapXlateValues);
profile1DType  string_to_profile1DType (std::string s_type);
profile1DField string_to_profile1DField(std::string s_field);


// Declaration of the class profile1DRawData
class profile1DRawData
{
private:

    void extractListTokens_TURBO_CSV(std::string s_buffer, std::list<std::string>             &l_tokens_);
    void extractListNameParam_Units (std::string s_buffer, std::string &nameParam, std::string &nameUnits);

    std::string nameFile_;
    std::string typeFile_;

    bool is_validFlag;

    // Data read from the file
    std::list<std::string> header_;
    std::string name_;
    std::string spatial_fields_;

    typedef struct {
        std::string         units;
        std::vector<double> values;
    } ProfileValues;

    typedef std::map<std::string, ProfileValues > m_t_Profile_;

    m_t_Profile_ profile_;


public:
    // Constructor
    profile1DRawData(std::string nameFile, std::string typeFile);

    bool is_valid()
    {
        return is_validFlag;
    }

    int get_values                 (std::string key, std::vector<double> &values, bool exitOnInvalidKey = true);
    int get_r                      (std::vector<double> &r);
    int get_z                      (std::vector<double> &z);
    int get_velocityAxial          (std::vector<double> &v_axial);
    int get_velocityRadial         (std::vector<double> &v_radial);
    int get_velocityCircumferential(std::vector<double> &v_circum);
    int get_pressure               (std::vector<double> &pressure);
    int get_tke                    (std::vector<double> &tke);
    int get_epsilon                (std::vector<double> &epsilon);
    int get_omega                  (std::vector<double> &omega);

    // Keys to identify the columns data in turboCSV file
    const static std::string KEY_R;
    const static std::string KEY_Z;
    const static std::string KEY_VELOCITY_AXIAL;
    const static std::string KEY_VELOCITY_RADIAL;
    const static std::string KEY_VELOCITY_CIRCUMFERENTIAL;
    const static std::string KEY_PRESSURE;
    const static std::string KEY_TKE;
    const static std::string KEY_EPSILON;
    const static std::string KEY_OMEGA;

    // Error codes
    const static int TURBO_CSV_INVALID_VALUE_KEY = -1;
};

} // End namespace Foam

#endif  // ifndef profile1DRawData_H


