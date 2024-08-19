# Hexer - Hexbin density and boundary generation

Hexer is a [LGPL] C++ library that provides some classes for generating 
hexbin density surfaces and [multipolygon] boundaries for large point sets. I use 
the code for generating boundary polygons of large [LiDAR] collections, but it is 
useful in other contexts.

Hexer also offers support for creating boundaries and density surfaces using Uber's [H3] 
geospatial indexing library. By specifying <b>--grid h3</b>, points can be summarized
inside of H3's existing global hexagon grid. H3 [resolution] can be specified with <b>-r</b>,
or Hexer can calculate an appropriate cell size based on the input. <b>H3 processing
currently only supports .LAS and .LAZ files with WGS84 (EPSG:4326) CRS</b>.

Hexer supports two operations at this time, <i>density</i> and <i>boundary</i>. You 
use hexer through the <b>curse</b> command:

```
[howardbutler@ardere hexer (master)]$ ./bin/curse --help
--------------------------------------------------------------------
    curse (hexer 1.4.0 at revision e2d559 with GDAL 3.9.1)
--------------------------------------------------------------------
  command [boundary]
      Command to run on points ('boundary' or 'density')

  input
      Input point set to curse

  output, o []
      Specify an OGR-compatible output filename to write boundary. stdout used if none specified.

  edge [0]
      Edge distance of hexagon (hexgrid only)

  count [0]
      Number of points that must be in polygon for it to be positive space

  resolution, r [-1]
      H3 grid resolution: 0 (coarsest) - 15 (finest)

  grid [hexgrid]
      Grid type ('hexgrid' or 'h3'): proprietary HexGrid hexagons, or H3 indexed grid

For more information, see the full documentation for hexer at:
 http://github.com/hobu/hexer

```
### H3 Processing
```
$ curse density mylasfile.las --output myhexagons.shp --grid h3
$ curse boundary mylasfile.las --output myboundary.shp --grid h3 -r 12
```
<img src="https://github.com/hobu/hexer/raw/master/images/h3bounds.png?raw=true"  alt="Uncompahgre boundary at H3 resolution 11" /></a>


### Default Processing
```
$ curse boundary mypointfile.shp --output myboundary.shp
$ curse density mylasfile.las --output myhexagons.shp --edge 100
```

<a href="http://a.tiles.mapbox.com/v3/hobu.serpent-mound.html#16.00/39.0346/-83.4353"><img src="https://github.com/hobu/hexer/raw/master/images/serpent.png?raw=true"  alt="Serpent Mound Hexbins" /></a>

[LGPL]: http://www.gnu.org/licenses/lgpl-2.1.html
[LiDAR]: https://en.wikipedia.org/wiki/LIDAR
[multipolygon]: http://en.wikipedia.org/wiki/Well-known_text
[H3]: https://h3geo.org/
[resolution]: https://h3geo.org/docs/core-library/restable 

[map]: http://a.tiles.mapbox.com/v3/hobu.serpent-mound.html#16.00/39.0346/-83.4353

