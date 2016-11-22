/*****************************************************************************
 * A x e l
 *****************************************************************************
 * QGeometricKernel
 * 2006-11-06
 * Julien Wintz
 *****************************************************************************
 *               Copyright (C) 2006 INRIA Sophia-Antipolis
 *****************************************************************************
 * Comments :         
 *
 ****************************************************************************/


# ifndef QGEOMETRICKERNEL_H
# define QGEOMETRICKERNEL_H


# include <QBrush>
# include <QFont>
# include <QImage>
# include <QPen>
# include <QImage>

# include <QAxel/QAxelObject.h>
# include <QAxel/QShape.h>

# include <QGeometricKernel/QGeometricInterface.h>

# include <geometrix/IShape.h>
# include <geometrix/IShapeFactory.h>

# include <GoTools/geometry/SplineCurve.h>
# include <GoTools/geometry/SplineSurface.h>
# include <GoTools/trivariate/SplineVolume.h>   

# include <QAxel/QHSVcolormap.h>


using namespace QAxel ;
using namespace qglviewer ;
using namespace Go;

class SISLCurve ;
class SISLSurf ;
//class SplineCurve ;

/****************************************************************************/
/*!
  \class QGeometricKernel QGeometricKernel.h
  \brief The class QGeometricKernel implements the \link
  QCoreInterface \endlink as a glue to the Synaps library.

  More ...
*/
/****************************************************************************/
class QGeometricKernel : public QObject, public QGeometricInterface
{
    Q_OBJECT
    Q_INTERFACES(QGeometricInterface QAxel::QPluginInterface QAxel::QInitializablePluginInterface)
	
public:
    QString name() const ;
    QStringList features() const ;

    void init(MainWindow * window) ;
	
    void create(QImplicitCurve * c) ;
    void create(QImplicitSurface * s) ;
    void create(QRationalCurve * c) ;
    void create(QRationalSurface * s) ;
    void create(QBSplineCurve * c) ;
    void create(QBSplineSurface * s) ;
    void create(QBSplineVolume * bv) ;
    void create(QBSplineVolume4f * bv) ;

    QPoint4f * eval(QExactSolutionVolume* bv, float u, float v, float w) ;
    QPoint3f * eval(QExactSolutionSurface* s, float u, float v) ;
    QPoint3f * eval(QRationalCurve * c, float u) ;
    QPoint3f * eval(QRationalSurface * s, float u, float v) ;
    void       eval(QRationalSurface * s, float u, float v, float * x, float * y, float * z) ;
    void       eval(QRationalSurface * s, float u, float v, double * x, double * y, double * z) ;
    QPoint3f * eval(QBSplineCurve * c, float u) ;
    void       eval(QBSplineCurve * c, float u, float * x, float * y, float * z) ;
    QPoint3f * eval(QBSplineSurface * s, float u, float v) ;
    void       eval(QBSplineSurface * s, float u, float v, float * x, float * y, float * z) ;
    void       eval(QBSplineSurface * s, float u, float v, double * x, double * y, double * z) ;
    QPoint3f * eval(QBSplineVolume * bv, float u, float v, float w) ;
    QPoint4f * eval(QBSplineVolume4f * bv, float u, float v, float w) ;
    QBSplineVolume * deriVolume(QBSplineVolume * bv, int ider1, int ider2, int ider3);
    QBSplineVolume * linearSweptVolume(QBSplineSurface * bs, QBSplineCurve *bc, QPoint3f * point);


    QShape * intersect(QAxelObject * s1, QAxelObject * s2);
    QShape * selfintersect(QAxelObject * s) ;
    QList<QPiecewiseLinearCurve *> selfIntersectionCurves(QShape * s) ;    
    
    void draw(QAxelObject * o) ;
    void draw(QProceduralSurface * s) ;
    void draw(QTextureSurface * s) ;
    void draw(QBSplineVolume * bv) ;  
    void draw(QBSplineVolume4f * bv) ; 
    void draw_z(QSurfaceColorMap * s) ;
    void draw_exact(QExactSolutionSurface * s) ;
    void draw_mean(QMeanCurvatureColorMap * s) ;
    void draw_mini(QGeneralMiniSurface* s) ;
    void draw_exactV(QExactSolutionVolume * v) ;

    int  nindexes(QShape * s) ;
    int  nvertices(QShape * s) ;
    void triangulate(QShape * s, int * nbi, int * indices, int * nbv, double * vertices) ;

    int  nindexes(QBSplineCurve * c) ;
    int  nvertices(QBSplineCurve * c) ;
    void triangulate(QBSplineCurve * c, int * nbi, int * indices, int * nbv, double * vertices) ;

    int  nindexes(QBSplineSurface * s) ;
    int  nvertices(QBSplineSurface * s) ;
    void triangulate(QBSplineSurface * s, int * nbi, int * indices, int * nbv, double * vertices, double * normals) ;

    /*! @name IOStream */    
    QBSplineCurve * readCurve(QFile * file) ;
    QBSplineSurface * readSurface(QFile * file) ;
    QList<QBSplineSurface *> readIGESSurface(QFile * file); 
    QList<QBSplineCurve *> readIGESCurve(QFile * file); 
    // QSimplexsplineSurface * readSimplexSurface(QFile * file);

    /*! @name Evaluation */
    QPoint3f * eval(QBSplineCurve * curve, float t, int derivative) ;
    QPoint3f * eval(QBSplineSurface * surface, float u, float v, int derivative) ;
    QPoint3f * normal(QBSplineSurface * surface, float u, float v) ;
    void       normal(QBSplineSurface * surface, float u, float v, double * x, double * y, double * z) ;
    QBSplineCurve * eval(QBSplineSurface * surface, float t, int dir) ;
    QBSplineCurve * updt(QBSplineCurve * curve, QBSplineSurface * surface, float t, int dir) ;

    QBSplineSurface * eval(QBSplineVolume * volume, float t, int dir);

    float area(QBSplineSurface * s) ;
    float IGAEnergy(QBSplineSurface * s) ;
    float harmonicEnergy(QBSplineSurface * s) ;
    float meanCurvature(QBSplineSurface * s, float u, float v) ;
    float GaussianCurvature(QBSplineSurface * s, float u, float v) ;
   

    /*! @name Closest points */
    QPoint3f * closestPoint(QBSplineCurve * curve, QPoint3f * p) ;
    
    /*! @name Approximation */
    QBSplineCurve * approximateBSplineCurve(QList<QPoint3f *> points, int , int , int, int) ;
    QBSplineSurface * approximateBSplineSurface(QList<QPoint3f *> points, int , int, int , int, int) ;

    /*! @name Joining */ 
    QBSplineCurve * join(QBSplineCurve * c1, QBSplineCurve * c2) ;
    
    /*! @name Interpolation */
    QBSplineCurve * interpolate(QList<QPoint3f *> points) ;
    // QBSplineSurface * interpolate(QList<QPoint3f *> points) ;
    
    /*! @name Blending */
    QBSplineCurve * blend(QBSplineCurve * curve1, QBSplineCurve * curve2) ;

    /*! @name Offset */
    QBSplineCurve   * offset(QBSplineCurve * curve, Vec direction, double distance) ;
    QBSplineSurface * offset(QBSplineSurface * surface, double distance) ;
    
    /*! @name Intersection */
    QList<QPoint3f *> intersection (QBSplineCurve * curve1, QBSplineCurve * c2) ;
    QList<QPoint3f *> intersection (QBSplineCurve * curve, QCell * cell) ;
    QList<QPoint3f *> intersection (QBSplineCurve * curve, QPair<QPoint3f *, QPoint3f *> line, double * normal) ;
    QList<QPoint3f *> intersection (QBSplineCurve * curve, QPlane * plane) ;
    QList<QPoint3f *> intersectionX(QBSplineCurve * c, float y, float xmin, float xmax) ;
    QList<QPoint3f *> intersectionY(QBSplineCurve * c, float x, float ymin, float ymax) ;

    QList<QPoint3f *> intersection(QPiecewiseLinearCurve * c, QPiecewiseLinearCurve * l) ;
    QList<QPoint3f *> intersection(QPiecewiseLinearCurve * c, QCell * cell) ;
    
    /*! @name Picking */
    QBSplineCurve * pick(QBSplineCurve * curve, float begin, float end) ;

    /*! @name Bounding */
    QBoundingBox * bounding(QBSplineCurve   * curve) ;
    QBoundingBox * bounding(QBSplineSurface * surface) ;

    /*! @name Extrusion */
    QBSplineSurface * extrude(QBSplineCurve * curve, QBSplineCurve * path, int sample) ;
    QBSplineSurface * extrudeCPF(QBSplineCurve * curve, QBSplineCurve * path1, QBSplineCurve * path2, int sample);
    QBSplineSurface * extrudeFree(QBSplineCurve * curve, QPoint3f * point, QBSplineCurve * path, int sample);

    /*! @name Lofting */
    QBSplineSurface * loft(QBSplineCurve * curve, int sample, QList<QPoint3f*>& points, QList<double>& radius) ;
    QBSplineSurface * loftFrenet(QBSplineCurve * curve, int sample, QList<QPoint3f*>& points, QList<double>& radius) ;

    void medial(QBSplineCurve * c1, QBSplineCurve * c2) ;

    void frenet(QBSplineCurve * curve, int sample) ;
    void frenet(QBSplineCurve * curve, double param, double* p, double* t, double* n, double* b) ;

    QPlane * tangentPlane(QBSplineSurface * surface, float u, float v) ;

    inline QMap<QAxelObject *, SISLCurve *> curvemap(void) {
	return curves ;
    }

    QMap<QAxelObject *, SISLSurf  *> surfacemap(void) {
	return surfaces ;
    }

private:
    void rotationMinimizationFrames(QBSplineCurve * curve, int sample, double * p, double * t, double * n, double * b) ;
    void rotationMinimizationFramesFree(QBSplineCurve * curve, QPoint3f * point,  int sample, double * p, double * t, double * n, double * b) ;
    void curvePairFrames(QBSplineCurve * curve1, QBSplineCurve *curve2, int sample) ;
    void frenetFrames(QBSplineCurve * curve, int sample, double * p, double * t, double * n, double * b) ;

    void getcolor(double c,  float* mc);
private:

    QBSplineCurve   * toQBSplineCurve(SISLCurve * sislcurve) ;
    QBSplineSurface * toQBSplineSurface(SISLSurf * sislsurface) ;
    double closestParameter(QBSplineCurve * curve, QPoint3f * p) ;

    QBSplineCurve   * GotoQBSplineCurve(SplineCurve & gtcurve);
    QBSplineSurface * GotoQBSplineSurface(SplineSurface & gtSurface);
    QBSplineVolume *  GotoQBSplineVolume(SplineVolume& gtvolume);
    QBSplineVolume4f *  GotoQBSplineVolume4f(SplineVolume& gtvolume);	
    
    SplineCurve *   QBSplineCurvetoGo(QBSplineCurve * c);
    SplineSurface * QBSplineSurfacetoGo(QBSplineSurface* s );
 
    SplineVolume *  QBSplineVolumetoGo(QBSplineVolume* v );
    SplineVolume *  QBSplineVolume4ftoGo(QBSplineVolume4f* v );

 private:

    QMap<QAxelObject *, SISLCurve *> curves ;
    QMap<QAxelObject *, SISLSurf  *> surfaces ;
    HSVColor hsv_colormap;//draw color map


 private:
    QMap<QAxelObject *, mmx::IShape *> map ;
    mmx::IShapeFactory * factory;

 
} ;

# endif
