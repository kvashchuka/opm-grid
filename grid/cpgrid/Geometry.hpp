//===========================================================================
//
// File: Geometry.hpp
//
// Created: Fri May 29 23:29:24 2009
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
Copyright 2009, 2010 SINTEF ICT, Applied Mathematics.
Copyright 2009, 2010 Statoil ASA.

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

#ifndef OPENRS_GEOMETRY_HEADER
#define OPENRS_GEOMETRY_HEADER

#include <boost/static_assert.hpp>

namespace Dune
{
    namespace cpgrid
    {

	/// This class encapsulates geometry for both vertices, intersections and cells.
	/// For vertices and cells we use the cube type, but without providing nonsingular
	/// global() and local() mappings. However, we do provide corner[s]().
	/// For intersections, we use the singular type, and no corners().
	template <int mydim, int cdim, class GridImp> // GridImp arg never used
	class Geometry
	{
	    BOOST_STATIC_ASSERT(cdim == 3);
	public:
	    /// Dimension of underlying grid.
	    enum { dimension = 3 };
            /// Dimension of domain space of \see global().
	    enum { mydimension = mydim };
            /// Dimension of range space of \see global().
	    enum { coorddimension = cdim };
            /// World dimension of underlying grid.
	    enum { dimensionworld = 3 };

            /// Coordinate element type.
	    typedef double ctype;

            /// Domain type of \see global().
	    typedef FieldVector<ctype, mydimension> LocalCoordinate;
            /// Range type of \see global().
	    typedef FieldVector<ctype, coorddimension> GlobalCoordinate;

            /// Type of Jacobian matrix.
	    typedef FieldMatrix< ctype, coorddimension, mydimension > 	Jacobian;
            /// Type of transposed Jacobian matrix.
	    typedef FieldMatrix< ctype, mydimension, coorddimension > 	JacobianTransposed;

	    /// @brief
	    /// @todo Doc me!
	    /// @tparam Doc me!
	    /// @param
	    Geometry(const GlobalCoordinate& pos,
		     ctype vol,
		     const GlobalCoordinate* allcorners = 0,
		     const int* corner_indicies = 0)
		: pos_(pos), vol_(vol), allcorners_(allcorners), cor_idx_(corner_indicies)
	    {
		ASSERT(mydimension != 3 || (allcorners && corner_indicies));
	    }

            /// Default constructor, giving a non-valid geometry.
	    Geometry()
		: pos_(0.0), vol_(0.0), allcorners_(0), cor_idx_(0)
	    {
	    }

	    /// In spite of claiming to be a cube geomety, we do not
	    /// make a 1-1 mapping from the reference cube to the cell.
	    const GlobalCoordinate& global(const LocalCoordinate&) const
	    {
		return pos_;
	    }

	    /// In spite of claiming to be a cube geomety, we do not
	    /// make a 1-1 mapping from the cell to the reference cube.
	    LocalCoordinate local(const GlobalCoordinate&) const
	    {
		LocalCoordinate dummy(0.0);
		return dummy;
	    }

	    /// @brief
	    /// @todo Doc me!
	    /// @param
	    /// @return
	    double integrationElement(const LocalCoordinate&) const
	    {
		return vol_;
	    }

	    /// Using the cube type for all entities now (cells and vertices),
	    /// but we use the singular type for intersections.
	    GeometryType type() const
	    {
		GeometryType t;
                t.makeCube(mydimension);
		return t;
	    }

	    /// The number of corners of this convex polytope.
	    /// Returning 8 or 1, depending on whether we are a hexahedron or not.
	    int corners() const
	    {
		if (mydimension == 3) {
		    return 8;
		} else if (mydimension == 0) {
		    return 1;
		} else {
                    THROW("Meaningless call to cpgrid::Geometry::corner(int): "
                          "singular geometry has no corners.");
		}
	    }

	    /// The corner method requires the points, which we may not necessarily want to provide.
	    /// We will need it for visualization purposes though. For now we throw.
	    GlobalCoordinate corner(int cor) const
	    {
		if (mydimension == 3) {
		    return allcorners_[cor_idx_[cor]];
		} else if (mydimension == 0) {
		    return pos_;
		} else {
                    THROW("Meaningless call to cpgrid::Geometry::corner(int): "
                          "singular geometry has no corners.");
		}
	    }

	    /// @brief
	    /// @todo Doc me!
	    /// @return
	    ctype volume() const
	    {
		return vol_;
	    }

	    /// Returns the centroid of the geometry.
	    const GlobalCoordinate& center() const
	    {
		return pos_;
	    }

	    /// @brief
	    /// @todo Doc me!
	    const FieldMatrix<ctype, mydimension, coorddimension>&
	    jacobianTransposed(const LocalCoordinate& local) const
	    {
		THROW("Meaningless to call jacobianTransposed() on singular geometries.");
		static FieldMatrix<ctype, mydimension, coorddimension> dummy;
		return dummy;
	    }

	    /// @brief
	    /// @todo Doc me!
	    const FieldMatrix<ctype, coorddimension, mydimension>&
	    jacobianInverseTransposed(const LocalCoordinate& /*local*/) const
	    {
		THROW("Meaningless to call jacobianInverseTransposed() on singular geometries.");
		static FieldMatrix<ctype, coorddimension, mydimension> dummy;
		return dummy;
	    }

	    /// The mapping implemented by this geometry is singular, and therefore not affine.
	    bool affine() const
	    {
		return false;
	    }

	private:
	    GlobalCoordinate pos_;
	    double vol_;
	    const GlobalCoordinate* allcorners_; // For dimension 3 only
	    const int* cor_idx_;               // For dimension 3 only
	};







	/// This class encapsulates geometry for both vertices, intersections and cells.
	/// For vertices and cells we use the cube type, but without providing nonsingular
	/// global() and local() mappings. However, we do provide corner[s]().
	/// For intersections, we use the singular type, and no corners().
        ///
        /// Specialization for 2 dimensional geometries, that is
        /// intersections (since codim 1 entities are not in CpGrid).
	template <int cdim, class GridImp> // GridImp arg never used
	class Geometry<2, cdim, GridImp>
	{
	    BOOST_STATIC_ASSERT(cdim == 3);
	public:
	    /// Dimension of underlying grid.
	    enum { dimension = 3 };
            /// Dimension of domain space of \see global().
	    enum { mydimension = 2 };
            /// Dimension of range space of \see global().
	    enum { coorddimension = cdim };
            /// World dimension of underlying grid.
	    enum { dimensionworld = 3 };

            /// Coordinate element type.
	    typedef double ctype;

            /// Domain type of \see global().
	    typedef FieldVector<ctype, mydimension> LocalCoordinate;
            /// Range type of \see global().
	    typedef FieldVector<ctype, coorddimension> GlobalCoordinate;

            /// Type of Jacobian matrix.
	    typedef FieldMatrix< ctype, coorddimension, mydimension > 	Jacobian;
            /// Type of transposed Jacobian matrix.
	    typedef FieldMatrix< ctype, mydimension, coorddimension > 	JacobianTransposed;

	    /// @brief
	    /// @todo Doc me!
	    /// @tparam Doc me!
	    /// @param
	    Geometry(const GlobalCoordinate& pos,
		     ctype vol)
		: pos_(pos), vol_(vol)
	    {
	    }

            /// Default constructor, giving a non-valid geometry.
	    Geometry()
		: pos_(0.0), vol_(0.0)
	    {
	    }

	    /// In spite of claiming to be a cube geomety, we do not
	    /// make a 1-1 mapping from the reference cube to the cell.
	    const GlobalCoordinate& global(const LocalCoordinate&) const
	    {
		return pos_;
	    }

	    /// In spite of claiming to be a cube geomety, we do not
	    /// make a 1-1 mapping from the cell to the reference cube.
	    LocalCoordinate local(const GlobalCoordinate&) const
	    {
		LocalCoordinate dummy(0.0);
		return dummy;
	    }

	    /// @brief
	    /// @todo Doc me!
	    /// @param
	    /// @return
	    double integrationElement(const LocalCoordinate&) const
	    {
		return vol_;
	    }

	    /// Using the cube type for all entities now (cells and vertices),
	    /// but we use the singular type for intersections.
	    GeometryType type() const
	    {
		GeometryType t;
                t.makeNone(mydimension);
		return t;
	    }

	    /// The number of corners of this convex polytope.
            /// Since this geometry is singular, we have no corners as such.
	    int corners() const
	    {
                return 0;
	    }

	    /// The corner method requires the points, which we may not necessarily want to provide.
	    /// We will need it for visualization purposes though. For now we throw.
	    GlobalCoordinate corner(int cor) const
	    {
                THROW("Meaningless call to cpgrid::Geometry::corner(int): "
                      "singular geometry has no corners.");
	    }

            /// Volume (area, actually) of intersection.
	    ctype volume() const
	    {
		return vol_;
	    }

	    /// Returns the centroid of the geometry.
	    const GlobalCoordinate& center() const
	    {
		return pos_;
	    }

	    /// @brief
	    /// @todo Doc me!
	    const FieldMatrix<ctype, mydimension, coorddimension>&
	    jacobianTransposed(const LocalCoordinate& local) const
	    {
		THROW("Meaningless to call jacobianTransposed() on singular geometries.");
	    }

	    /// @brief
	    /// @todo Doc me!
	    const FieldMatrix<ctype, coorddimension, mydimension>&
	    jacobianInverseTransposed(const LocalCoordinate& /*local*/) const
	    {
		THROW("Meaningless to call jacobianInverseTransposed() on singular geometries.");
	    }

	    /// The mapping implemented by this geometry is singular, and therefore not affine.
	    bool affine() const
	    {
		return false;
	    }

	private:
	    GlobalCoordinate pos_;
	    ctype vol_;
	};


    } // namespace cpgrid
} // namespace Dune

#endif // OPENRS_GEOMETRY_HEADER
