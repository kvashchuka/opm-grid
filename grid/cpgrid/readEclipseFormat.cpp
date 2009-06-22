//===========================================================================
//
// File: readEclipseFormat.cpp
//
// Created: Fri Jun 12 09:16:59 2009
//
// Author(s): Atgeirr F Rasmussen <atgeirr@sintef.no>
//            B�rd Skaflestad     <bard.skaflestad@sintef.no>
//
// $Date$
//
// $Revision$
//
//===========================================================================

/*
Copyright 2009 SINTEF ICT, Applied Mathematics.
Copyright 2009 Statoil ASA.

This file is part of The Open Reservoir Simulator Project (OpenRS).

OpenRS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenRS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OpenRS.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <fstream>
#include "../CpGrid.hpp"
#include "EclipseGridParser.hpp"
#include "EclipseGridInspector.hpp"
#include "../preprocess/preprocess.h"

namespace Dune
{


    /// Read the Eclipse grid format ('.grdecl').
    void CpGrid::readEclipseFormat(const std::string& filename, double z_tolerance)
    {
	// Read eclipse file data.
	EclipseGridParser parser(filename);
	EclipseGridInspector inspector(parser);

	// Make input struct for processing code.
	grdecl g;
	g.dims[0] = inspector.gridSize()[0];
	g.dims[1] = inspector.gridSize()[1];
	g.dims[2] = inspector.gridSize()[2];
	g.coord = &(parser.getFloatingPointValue("COORD")[0]);
	g.zcorn = &(parser.getFloatingPointValue("ZCORN")[0]);
	g.actnum = &(parser.getIntegerValue("ACTNUM")[0]);

	// Process.
	processed_grid output;
	process_grdecl(&g, z_tolerance, &output);

	// Move data into the grid's structures.
	// Build face_to_cell_
	int nf = output.number_of_faces;
	cpgrid::EntityRep<0> cells[2];
	for (int i = 0; i < nf; ++i) {
	    const int* fnc = output.face_neighbors + 2*i;
	    int cellcount = 0;
	    if (fnc[0] != -1) {
		cells[cellcount].setValue(fnc[0], true);
		++cellcount;
	    }
	    if (fnc[1] != -1) {
		cells[cellcount].setValue(fnc[1], false);
		++cellcount;
	    }
	    ASSERT(cellcount == 1 || cellcount == 2);
	    face_to_cell_.appendRow(cells, cells + cellcount);
	}
	// Build cell_to_face_
	face_to_cell_.makeInverseRelation(cell_to_face_);
	// Build cell_to_point_ [skipped for now]
	// Build geometry_
	
	// Build face_normals_

	// Clean up the output struct.
	free_processed_grid(&output);
    }

} // namespace Dune

