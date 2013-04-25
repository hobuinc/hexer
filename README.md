# Hexer - Hexbin density and boundary generation

Hexer is a [LGPL] C++ library that provides some classes for generating 
hexbin density surfaces and [multipolygon] boundaries for large point sets. I use 
the code for generating boundary polygons of large [LiDAR] collections, but it is 
useful in other contexts.

Hexer supports two operations at this time, <i>density</i> and <i>boundary</i>. You 
use hexer through the <b>curse</b> command:

```
[howardbutler@ardere hexer (master)]$ ./bin/curse --help
--------------------------------------------------------------------
    curse (hexer 1.0.0 at revision eca953 with GDAL 1.10dev)
--------------------------------------------------------------------

Command:
  --input arg           Input point set to curse
  --command arg         Command to run on points ('boundary' or 'density')
  --output arg          Specify an OGR-compatible output filename to write
                        boundary. stdout used if none specified.

Basic:
  -h [ --help ]         This help message
  --version             Show version info

Boundary:
  --edge arg (=0)       Edge distance of hexagon
  --count arg (=0)      Number of points that must be in polygon for it to be
                        positive space

For more information, see the full documentation for hexer at:
 http://github.com/hobu/hexer
```

```
$ curse boundary mypointfile.shp --output myboundary.shp
$ curse density mylasfile.las --output myhexagons.shp --edge 100
```

<a href="href="http://a.tiles.mapbox.com/v3/hobu.serpent-mound.html#16.00/39.0346/-83.4353""><img src="https://github.com/hobu/hexer/raw/master/images/serpent.png?raw=true"  alt="Serpent Mound Hexbins" /></a>

[LGPL]: http://www.gnu.org/licenses/lgpl-2.1.html
[LiDAR]: https://en.wikipedia.org/wiki/LIDAR
[multipolygon]: http://en.wikipedia.org/wiki/Well-known_text

[map]: http://a.tiles.mapbox.com/v3/hobu.serpent-mound.html#16.00/39.0346/-83.4353

