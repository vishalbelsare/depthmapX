// Copyright (C) 2017 Christian Sailer

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.



#ifndef MODEPARSERREGISTRY_H
#define MODEPARSERREGISTRY_H

#include "imodeparser.h"
#include "imodeparserfactory.h"
#include <vector>
#include <memory>

class ModeParserRegistry : public IModeParserFactory
{
public:
    ModeParserRegistry()
    {
        populateParsers();
    }

    const ModeParserVec &getModeParsers() const {return _availableParsers;}
private:
    void populateParsers();
    ModeParserVec _availableParsers;
};

#define REGISTER_PARSER(parser)\
    _availableParsers.push_back(std::unique_ptr<IModeParser>(new parser));

#endif // MODEPARSERREGISTRY_H
