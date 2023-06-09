#ifndef POLYGONS_H_
#define POLYGONS_H_

#include <list>
#include <iostream>


// TODO -- Will need to rewrite/specialize our hashing
// function for our polygons, as they are not guaranteed
// a unique way of printing.

namespace Semiring
{
	namespace Polyhedral
	{
		class PolygonCollection;

		struct Point
		{
			double x;
			double y;

			Point()
			{
				x = 0;
				y = 0;
			}

			Point(double nX, double nY)
			{
				x = nX;
				y = nY;
			}

			Point& operator=(const Point& rhs)
			{
				x = rhs.x;
				y = rhs.y;

				return *this;
			}
		};

		struct Ring
		{
			// Vertices stored in counterclockwise order (interior is to the right of the line)
			std::list<Point> vertices;

			// Flag to mark if the edges are included as part of the ring
			bool closed;

			// If this is false then the ring is assumed to be a bounded ring
			// If it is true then the two endpoints are rays and mark direction to shoot to infinity
			bool ray;

			Ring()
			{
				closed = true;
				ray = false;
			}

			Ring& operator=(const Ring& rhs)
			{
				closed = rhs.closed;
				vertices.clear();
				for (auto p : rhs.vertices)
				{
					Point nP = p;
					vertices.push_back(nP);
				}

				return *this;
			}
		};

		// Has list of points outlining exterior boundary and vector of list of points
		// outlining different interior boundaries
		// 
		// Is one contiguous body
		class Polygon
		{
		private:
			long double boundingBox;
			Ring boundary;
			std::list<Ring> interiorRings;
		public:
			Polygon();
			Polygon(Ring bound);

			void AdjustBoundingBox(long double nbb);
			Polygon Transpose() const;
			// Applies the matrix [[a,b],[c,d]];
			Polygon ApplyMatrix(double a, double b, double c, double d) const;

			inline long double GetBoundingBox() { return boundingBox; };

			friend bool SubsetEq(const Polygon& lhs, const Polygon& rhs);

			friend bool Overlap(const Polygon& lhs, const Polygon& rhs);

			friend Polygon Union(const Polygon& lhs, const Polygon& rhs);

			friend PolygonCollection Intersect(const Polygon& lhs, const Polygon& rhs);

			friend PolygonCollection SetDifference(const Polygon& lhs, const Polygon& rhs);

			friend PolygonCollection Multiply(const Polygon& lhs, const Polygon& rhs);

			Polygon& operator= (const Polygon& rhs);

			inline friend bool operator<=(const Polygon& lhs, const Polygon& rhs)
			{
				return SubsetEq(lhs, rhs);
			}

			inline friend bool operator==(const Polygon& lhs, const Polygon& rhs) 
			{
				return (lhs <= rhs) && (rhs <= lhs);
			}

			inline friend bool operator!=(const Polygon& lhs, const Polygon& rhs) 
			{
				return !(lhs == rhs);
			}

			inline friend bool operator>=(const Polygon& lhs, const Polygon& rhs)
			{
				return rhs <= lhs;
			}

			inline friend bool operator<(const Polygon& lhs, const Polygon& rhs)
			{
				return (lhs <= rhs) && !(rhs <= lhs);
			}

			inline friend bool operator>(const Polygon& lhs, const Polygon& rhs)
			{
				return (lhs >= rhs) && !(rhs >= lhs);
			}

			friend std::ostream& operator<<(std::ostream& os, const Polygon& ts)
			{
				os << "[";
				// TODO: Print polygon
				if (ts.boundary.ray)
					os << "<";
				if (!ts.boundary.closed)
					os << "~";
				for (auto p : ts.boundary.vertices)
				{
					os << "(" << (abs(p.x) < ts.boundingBox ? std::to_string(p.x) : p.x > 0 ? "inf" : "-inf") << "," << (abs(p.y) < ts.boundingBox ? std::to_string(p.y) : p.y > 0 ? "inf" : "-inf") << ")";
				}
				if (!ts.boundary.closed)
					os << "~";
				if (ts.boundary.ray)
					os << ">";

				for (auto iR : ts.interiorRings)
				{
					os << "|";

					if (iR.ray)
					{
						os << "<";
					}

					if (!iR.closed)
						os << "~";

					for (auto p : iR.vertices)
					{
						os << "(" << (abs(p.x) < ts.boundingBox ? std::to_string(p.x) : p.x > 0 ? "inf" : "-inf") << "," << (abs(p.y) < ts.boundingBox ? std::to_string(p.y) : p.y > 0 ? "inf" : "-inf") << ")";
					}

					if (!iR.closed)
						os << "~";
					if (iR.ray)
					{
						os << ">";
					}
				}

				os << "]";
				return os;
			}

			static Polygon UnitBox();
			static Polygon HalfPlane(Point, Point);
			static Polygon Line(Point, Point);
			static Polygon Dot(Point);
			// TODO also do line, half plane, and point
		};

		// List of disjoint polygons -- each polygon must be fully disjoint from one another
		// i.e. if they overlap at even one point then they must be contained in a single polygon
		class PolygonCollection
		{
		private:
			std::list<Polygon> polygons;
		public:
			PolygonCollection()
			{

			}

			// Add in a polygon to the collection
			PolygonCollection& Add(const Polygon add);

			// Check that a given polygon is contained in the collection
			friend bool SubsetEq(const Polygon& lhs, const PolygonCollection& rhs);

			// Check that our collection is contained in a given polygon
			friend bool SubsetEq(const PolygonCollection& lhs, const Polygon& rhs);

			// Check that our collection is contained in another collection
			friend bool SubsetEq(const PolygonCollection& lhs, const PolygonCollection& rhs);

			friend PolygonCollection Union(const PolygonCollection& lhs, const PolygonCollection& rhs);

			friend PolygonCollection Intersect(const PolygonCollection& lhs, const PolygonCollection& rhs);

			friend PolygonCollection SetDifference(const PolygonCollection& lhs, const PolygonCollection& rhs);

			PolygonCollection ApplyMatrix(double a, double b, double c, double d) const;
			PolygonCollection Transpose() const;

			// Treat the polygons as RxR matrices and multiply them
			friend PolygonCollection Multiply(const PolygonCollection& lhs, const PolygonCollection& rhs);

			PolygonCollection& operator= (const PolygonCollection& rhs);

			inline friend bool operator<=(const PolygonCollection& lhs, const PolygonCollection& rhs)
			{
				return SubsetEq(lhs, rhs);
			}

			inline friend bool operator==(const PolygonCollection& lhs, const PolygonCollection& rhs) 
			{
				return (lhs <= rhs) && (rhs <= lhs);
			}

			inline friend bool operator!=(const PolygonCollection& lhs, const PolygonCollection& rhs) 
			{
				return !(lhs == rhs);
			}

			inline friend bool operator>=(const PolygonCollection& lhs, const PolygonCollection& rhs)
			{
				return rhs <= lhs;
			}

			inline friend bool operator<(const PolygonCollection& lhs, const PolygonCollection& rhs)
			{
				return (lhs <= rhs) && !(rhs <= lhs);
			}

			inline friend bool operator>(const PolygonCollection& lhs, const PolygonCollection& rhs)
			{
				return (lhs >= rhs) && !(rhs >= lhs);
			}

			friend std::ostream& operator<<(std::ostream& os, const PolygonCollection& ts)
			{
				bool after = false;
				os << "{";
				for (auto p : ts.polygons)
				{
					if (after)
						os << ", ";
					else
						after = true;
					os << p;
				}

				os << "}";
				return os;
			}
		};

	};
};

#endif
