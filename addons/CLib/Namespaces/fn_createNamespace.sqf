#include "macros.hpp"
/*
    Community Lib - CLib

    Author: joko // Jonas

    Description:
    Create a Location

    Parameter(s):
    Is Global <Bool> (default: false)

    Returns:
    Namespace <Location>

    Example:
    -
*/

#define POS [-2000, -2000, -2000]

params [["_isGlobal", false]];

private _ret = if (_isGlobal isEqualType true && {_isGlobal}) then {
    private _grp = call CFUNC(getLogicGroup);
    _grp createUnit ["Logic", POS, [], 0, "NONE"];
} else {
    createLocation ["fakeTown", POS, 0, 0];
};

if (isNil QGVAR(allCustomNamespaces)) then {
    GVAR(allCustomNamespaces) = [];
};
GVAR(allCustomNamespaces) pushBack _ret;

_ret
