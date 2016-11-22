
/*****************************************************************************
 * A x e l
 *****************************************************************************
 * QGeometricKernel
 * 2006-11-06
 * Julien Wintz
 * Gang Xu
 *****************************************************************************
 *               Copyright (C) 2006 INRIA Sophia-Antipolis
 *****************************************************************************
 * Comments :         
 ****************************************************************************/


# include <QtGui>
# include <QImage>
# include <QPainter>
# include <QTextStream>
# include <QFont>
# include <QPen>
# include <QPixmap>


# include <QAxel/ObjectManager.h>
# include <QAxel/ToolManager.h>
# include <QAxel/QAxelObject.h>
# include <QAxel/QArrow.h>
# include <QAxel/QBoundingBox.h>
# include <QAxel/QSingularity.h>
# include <QAxel/QShape.h>
# include <QAxel/QPoint3f.h>
# include <QAxel/QPoint4f.h>
# include <QAxel/QVertex.h>
# include <QAxel/QEdge.h>
# include <QAxel/QFace.h>
# include <QAxel/QMesh.h>
# include <QAxel/QPlane.h>
# include <QAxel/QCurve.h>
# include <QAxel/QPiecewiseLinearCurve.h>
# include <QAxel/QSurface.h>
# include <QAxel/QVolume.h>
# include <QAxel/QBSplineVolume.h>
# include <QAxel/QBSplineVolume4f.h>
# include <QAxel/QSurfaceColorMap.h>
# include <QAxel/QExactSolutionSurface.h>
# include <QAxel/QExactSolutionVolume.h>
# include <QAxel/QBSplineSurface.h>
# include <QAxel/QMeanCurvatureColorMap.h>
# include <QAxel/QTextureSurface.h>
# include <QAxel/QHSVcolormap.h>
# include <QAxel/QPointSet.h>
# include <QAxel/QQuad.h>
# include <QAxel/QGeneralMiniSurface.h>
   
# include <QSplineKernel/QSplineKernel.h>
# include <iostream>
# include <fstream>

# include <geometrix/curves/SISLBSplineCurve.h>
# include <geometrix/curves/RationalCurve.h>
# include <geometrix/surfaces/SISLBSplineSurface.h>
# include <geometrix/surfaces/RationalSurface.h>
# include <geometrix/WShapeFactory.h>

# include <geometrix/IGLGood.h>
# include <geometrix/curves/IRationalCurve.h>
# include <geometrix/surfaces/IRationalSurface.h>
# include <geometrix/curves/IBSplineCurve.h>
# include <geometrix/surfaces/IBSplineSurface.h>
# include <geometrix/surfaces/IParametricSurface.h>

# include <sisl/sisl.h>
# include <sisl/sislP.h>
# include <sisl/GoReadWrite.h>


# include <GoTools/geometry/SplineCurve.h>
# include <GoTools/geometry/SISLconversion.h>
# include <GoTools/geometry/SplineSurface.h>
# include <GoTools/geometry/CurvatureAnalysis.h>
# include <GoTools/trivariate/SplineVolume.h>
# include <GoTools/trivariate/SweepVolumeCreator.h>
# include <GoTools/utils/Point.h>
# include <GoTools/igeslib/IGESconverter.h>

# include <QGLViewer/qglviewer.h>

# include <QGui/Viewer.h>

# define FG_RED      "\033[0;31m"
# define FG_MAGENTA  "\033[0;35m"
# define FG_GREEN    "\033[0;32m"
# define FG_BD       "\033[1m"
# define FG_UL       "\033[4m"
# define NOCOLOR     "\033[0m"

typedef mmx::meta::gentlist< 
    // MPolImplicitSurface,
    mmx::RationalCurve<double,3>, 
    mmx::RationalSurface<double>,
    mmx::SISLBSplineCurve,
    mmx::SISL::BSPS
>::T TypeList ;

using namespace std ;
using namespace qglviewer ;
using namespace mmx;
using namespace Go;

QBSplineCurve * QGeometricKernel::toQBSplineCurve(SISLCurve * sislcurve)
{
    QBSplineCurve * c = new QBSplineCurve ;
    c->number = sislcurve->in ;
    c->order  = sislcurve->ik ;
    c->knots  = sislcurve->et ;
    for(int i = 0 ; i < sislcurve->in ; i++)
	if(sislcurve->idim == 2)
	    c->push_point(new QPoint3f(sislcurve->ecoef[3*i+0], sislcurve->ecoef[3*i+1], 0)) ;
	else
	    c->push_point(new QPoint3f(sislcurve->ecoef[3*i+0], sislcurve->ecoef[3*i+1], sislcurve->ecoef[3*i+2])) ;
    c->ikind = sislcurve->ikind ;
    c->idim  = sislcurve->idim ;
    c->icopy = 1 ;

    c->compute() ;
    
    return c ;
}

QBSplineSurface * QGeometricKernel::toQBSplineSurface(SISLSurf * sislsurface)
{
    QBSplineSurface * s = new QBSplineSurface ;
    s->number1 = sislsurface->in1 ;
    s->number2 = sislsurface->in2 ;
    s->order1  = sislsurface->ik1 ;
    s->order2  = sislsurface->ik2 ;
    s->knots1  = sislsurface->et1 ;
    s->knots2  = sislsurface->et2 ;
    for(int i = 0 ; i < sislsurface->in1*sislsurface->in2 ; i++)
	s->push_point(new QPoint3f(sislsurface->ecoef[3*i+0], sislsurface->ecoef[3*i+1], sislsurface->ecoef[3*i+2])) ;
    s->ikind = sislsurface->ikind ;
    s->idim  = sislsurface->idim ;
    s->icopy = 1 ;

    s->compute() ;

    return s ;
}



QBSplineCurve * QGeometricKernel::GotoQBSplineCurve(SplineCurve& gtcurve)
{
  SISLCurve* sislcurve=Curve2SISL(gtcurve, true);
  QBSplineCurve * cv=toQBSplineCurve(sislcurve);
  return cv;
}



QBSplineSurface * QGeometricKernel::GotoQBSplineSurface(SplineSurface& gtsurface)
{
  SISLSurf* sislsurface=GoSurf2SISL(gtsurface, true);
  QBSplineSurface * cs=toQBSplineSurface(sislsurface);
  return cs;
}


QBSplineVolume * QGeometricKernel::GotoQBSplineVolume(SplineVolume& gtvolume)
{ 
    QBSplineVolume * v = new QBSplineVolume ;
    v->number1 = gtvolume.numCoefs(0) ;
    v->number2 = gtvolume.numCoefs(1) ;
    v->number3 = gtvolume.numCoefs(2) ;
    v->order1  = gtvolume.order(0) ;
    v->order2  = gtvolume.order(1) ;
    v->order3  = gtvolume.order(2) ;
    v->knots1  = const_cast<double*>(&(*(gtvolume.basis(0).begin()))) ;
    v->knots2  = const_cast<double*>(&(*(gtvolume.basis(1).begin()))) ;
    v->knots3  = const_cast<double*>(&(*(gtvolume.basis(2).begin()))) ;

    std::vector<double>::const_iterator coefsstart;
    int ikind;
   
    if (gtvolume.rational()) {
	coefsstart = gtvolume.rcoefs_begin();
	ikind = 2;
    } else {
	coefsstart = gtvolume.coefs_begin();
	ikind = 1;
    }

    double * ecoef =  const_cast<double*>(&(*coefsstart));

    for(int i = 0 ; i < gtvolume.numCoefs(0)* gtvolume.numCoefs(1)* gtvolume.numCoefs(2) ; i++)
	v->push_point(new QPoint3f(ecoef[3*i+0], ecoef[3*i+1], ecoef[3*i+2])) ;
    v->ikind = ikind ;
    v->idim  = gtvolume.dimension(); 
    v->icopy = 1 ;

    v->compute() ;

    return v ;
 
}

QBSplineVolume4f * QGeometricKernel::GotoQBSplineVolume4f(SplineVolume& gtvolume)
{ 
    QBSplineVolume4f * v = new QBSplineVolume4f ;
    v->number1 = gtvolume.numCoefs(0) ;
    v->number2 = gtvolume.numCoefs(1) ;
    v->number3 = gtvolume.numCoefs(2) ;
    v->order1  = gtvolume.order(0) ;
    v->order2  = gtvolume.order(1) ;
    v->order3  = gtvolume.order(2) ;
    v->knots1  = const_cast<double*>(&(*(gtvolume.basis(0).begin()))) ;
    v->knots2  = const_cast<double*>(&(*(gtvolume.basis(1).begin()))) ;
    v->knots3  = const_cast<double*>(&(*(gtvolume.basis(2).begin()))) ;

    std::vector<double>::const_iterator coefsstart;
    int ikind;
   
    if (gtvolume.rational()) {
	coefsstart = gtvolume.rcoefs_begin();
	ikind = 2;
    } else {
	coefsstart = gtvolume.coefs_begin();
	ikind = 1;
    }

    double * ecoef =  const_cast<double*>(&(*coefsstart));

    for(int i = 0 ; i < gtvolume.numCoefs(0)* gtvolume.numCoefs(1)* gtvolume.numCoefs(2) ; i++)
      v->push_point(new QPoint4f(ecoef[4*i+0], ecoef[4*i+1], ecoef[4*i+2], ecoef[4*i+3])) ;
    v->ikind = ikind ;
    v->idim  = gtvolume.dimension(); 
    v->icopy = 1 ;

    v->compute() ;

    return v ;
 
}


SplineCurve *  QGeometricKernel::QBSplineCurvetoGo(QBSplineCurve * c)
{
    double points[3*c->points.size()] ;
    for(int i = 0 ; i<c->points.size() ; i++)
      {
	points[3*i+0] = c->points[i]->x() ;
	points[3*i+1] = c->points[i]->y() ;
	points[3*i+2] = c->points[i]->z() ;
    }
    SISLCurve * sislcurve = newCurve(c->points.size(), c->order, c->knots, points, c->ikind, c->idim, 1) ;
    SplineCurve* sc= SISLCurve2Go(sislcurve);
    return sc;
}  


SplineSurface *  QGeometricKernel::QBSplineSurfacetoGo(QBSplineSurface * s)
{
    double points[s->points.size()*3] ;
    for(int i = 0 ; i<s->points.size() ; i++)
      {
	points[3*i+0] = s->points[i]->x() ;
	points[3*i+1] = s->points[i]->y() ;
	points[3*i+2] = s->points[i]->z() ;
    }
    SISLSurf * sislsurface = newSurf(s->number1, s->number2, s->order1,  s->order2, s->knots1,  s->knots2, points, s->ikind, s->idim, 1) ;
    SplineSurface* ss= SISLSurf2Go(sislsurface);
    return ss;
}


SplineVolume *  QGeometricKernel::QBSplineVolumetoGo(QBSplineVolume * v)
{
    double points[v->points.size()*3] ;
    for(int i = 0 ; i<v->points.size() ; i++)
      {
	points[3*i+0] =v->points[i]->x() ;
	points[3*i+1] =v->points[i]->y() ;
	points[3*i+2] =v->points[i]->z() ;
      }
   
    bool rational=false;
 
    return new SplineVolume(v->number1, v->number2, v->number3, v->order1, v->order2,v->order3,
			    v->knots1, v->knots2,v->knots3, points, v->idim, rational);

}


SplineVolume *  QGeometricKernel::QBSplineVolume4ftoGo(QBSplineVolume4f * v)
{
    double points[v->points.size()*4] ;
    for(int i = 0 ; i<v->points.size() ; i++)
      {
	points[4*i+0] =v->points[i]->s() ;
	points[4*i+1] =v->points[i]->t() ;
	points[4*i+2] =v->points[i]->u() ;
        points[4*i+3] =v->points[i]->v() ;
      }
   
    bool rational=false;
 
    return new SplineVolume(v->number1, v->number2, v->number3, v->order1, v->order2,v->order3,
			    v->knots1, v->knots2,v->knots3, points, 4, rational);

}




void printSislCurve(SISLCurve * sislcurve)
{
    cout << FG_GREEN << endl ;
    cout << "=== Sisl Curve ===" << endl ;
    cout << NOCOLOR << endl ;
    cout << "Dimension: " << sislcurve->idim << endl ;
    cout << "Number of control points: " << sislcurve->in << endl ;
    cout << "Order: " << sislcurve->ik << endl ;
    cout << "Knot vector: " << flush ; for(int i = 0 ; i < sislcurve->in+sislcurve->ik ; i++) cout << " " << sislcurve->et[i] ; cout << endl ;
    cout << "Control points:" << endl ;
    for(int i = 0 ; i < sislcurve->in ; i++)
	cout << sislcurve->ecoef[3*i+0] << " " << sislcurve->ecoef[3*i+1] << " " << sislcurve->ecoef[3*i+2] << endl ;
}

void printSislIntCurve(SISLIntcurve * sislintcurve)
{
    cout << FG_MAGENTA << endl ;
    cout << "=== Sisl Int Curve ===" << endl ;
    cout << NOCOLOR << endl ;
    cout << "Number of guide points: " << sislintcurve->ipoint << endl ;
    cout << "Number of parameter directions for first  object: " << sislintcurve->ipar1 << endl ;
    cout << "Number of parameter directions for second object: " << sislintcurve->ipar2 << endl ;
    cout << "Type of curve: " << sislintcurve->itype << endl ;
}

void scaleSislCurve(SISLCurve * sislcurve, float s)
{
    for(int i = 0 ; i < sislcurve->in ; i++) {
	sislcurve->ecoef[3*i+0] *= s ;
	sislcurve->ecoef[3*i+1] *= s ;
	sislcurve->ecoef[3*i+2] *= s ;
    }
}

void translateSislCurve(SISLCurve * sislcurve, float dx, float dy, float dz)
{
    for(int i = 0 ; i < sislcurve->in ; i++) {
	sislcurve->ecoef[3*i+0] += dx ;
	sislcurve->ecoef[3*i+1] += dy ;
	sislcurve->ecoef[3*i+2] += dz ;
    }
}

void gldraw(IGLGood * glg)
{
    if(!glg->vertices) return ;
    
    glEnableClientState(GL_VERTEX_ARRAY) ;
    glVertexPointer(3, GL_DOUBLE, 0, glg->vertices) ;

    if(glg->normals) { 
	glEnableClientState(GL_NORMAL_ARRAY) ;   
	glNormalPointer(GL_DOUBLE, 0, glg->normals) ; 
    }
 
    switch ( glg->type ) {
    case IGLGood::E_VERTEX:
	glDisable(GL_LIGHTING) ;
	glDrawElements(GL_POINTS, glg->nbi, GL_UNSIGNED_INT, glg->indexes) ;
	glEnable(GL_LIGHTING) ;
	break;
	
    case IGLGood::E_LINE:
	glDisable(GL_LIGHTING) ;
        glLineWidth(2.0);
	glDrawElements(GL_LINES, glg->nbi, GL_UNSIGNED_INT, glg->indexes) ;
	glEnable(GL_LIGHTING) ;
	break;
	
    case IGLGood::E_TRIANGLE:
	glDrawElements(GL_TRIANGLES, glg->nbi, GL_UNSIGNED_INT, glg->indexes) ;
	break;
	
    case IGLGood::E_QUAD:
	glDrawElements(GL_QUADS, glg->nbi, GL_UNSIGNED_INT, glg->indexes) ;
	break;
    }
    
    glDisableClientState(GL_VERTEX_ARRAY) ;
    
    if(glg->normals) glDisableClientState(GL_NORMAL_ARRAY) ;
}

bool contains(QList<QPoint3f *> l, QPoint3f * p)
{
    foreach(QPoint3f * e, l)
	if(p->equals(e))
	    return true ;
    return false ;
}

QString QGeometricKernel::name() const
{
    return "GeometricKernel" ;
}

QStringList QGeometricKernel::features(void) const
{
    return QStringList() << "" ;
}

void QGeometricKernel::init(MainWindow * window)
{
    factory = new WShapeFactory<TypeList> ;
}

// void QGeometricKernel::create(QImplicitCurve * c)
// {
// }
// 
// void QGeometricKernel::create(QImplicitSurface * s)
// {
// 	IShape * shape = factory->interface( "mmx::IImplicitSurface", 0 ) ;
// 	IImplicitSurface * is = dynamic_cast<IImplicitSurface *>(shape) ;
// 	map.insert(c, shape) ;
// }
 

void QGeometricKernel::create(QRationalCurve * c)
{
    IShape * shape = factory->iface( "mmx::IRationalCurve", 0 ) ;
    IRationalCurve * rc = dynamic_cast<IRationalCurve *>(shape) ;
    rc->setEquations(
        c->pw().toAscii().constData(),
        c->px().toAscii().constData(),
        c->py().toAscii().constData(),
        c->pz().toAscii().constData()
	) ;
    rc->setRange(c->tmin(), c->tmax()) ;
    map.insert(c, shape) ;
}

void QGeometricKernel::create(QRationalSurface * s)
{
    IShape * shape = factory->iface( "mmx::IRationalSurface", 0) ;
    IRationalSurface * rs = dynamic_cast<IRationalSurface *>(shape) ;
    rs->setEquations(
        s->pw().toAscii().constData(),
        s->px().toAscii().constData(),
        s->py().toAscii().constData(),
        s->pz().toAscii().constData(),
	s->variables().toAscii().constData()
    ) ;
    std::cout<< "Create RS" <<std::endl;
    rs->setRange(s->umin(), s->umax(), s->vmin(), s->vmax()) ;
    IParametricSurface * ps = dynamic_cast<IParametricSurface *>(shape) ;
    ps->setGrid(s->gridU(),s->gridV()) ;
    map.insert(s, shape) ;
}

void QGeometricKernel::create(QBSplineCurve * c)
{
    double points[3*c->points.size()] ;
    for(int i = 0 ; i<c->points.size() ; i++) {
	points[3*i+0] = c->points[i]->x() ;
	points[3*i+1] = c->points[i]->y() ;
	points[3*i+2] = c->points[i]->z() ;
    }
    SISLCurve * sislcurve = newCurve(c->points.size(), c->order, c->knots, points, c->ikind, c->idim, 1) ;
    sislcurve->cuopen = c->cuopen ;
    if(curves.contains(c))
	freeCurve(curves.value(c)) ;
    curves.insert(c, sislcurve) ;
    c->setBoundingBox(bounding(c)) ;

    IShape * shape = factory->iface( "mmx::IBSplineCurve", 0 ) ;
    IBSplineCurve * bc = dynamic_cast<IBSplineCurve *>(shape) ;
    bc->setDefinition(c->points.size(), c->order, c->knots, points) ;
    map.insert(c, shape) ;
}

void QGeometricKernel::create(QBSplineSurface * s)
{
    double points[s->points.size()*3] ;
    for(int i = 0 ; i<s->points.size() ; i++) {
	points[3*i+0] = s->points[i]->x() ;
	points[3*i+1] = s->points[i]->y() ;
	points[3*i+2] = s->points[i]->z() ;
    }
    SISLSurf * sislsurface = newSurf(s->number1, s->number2, s->order1,  s->order2, s->knots1,  s->knots2, points, s->ikind, s->idim, 1) ;
    if(surfaces.contains(s))
	freeSurf(surfaces.value(s)) ;
    surfaces.insert(s, sislsurface) ;
    s->setBoundingBox(bounding(s)) ;

    IShape * shape = factory->iface( "mmx::IBSplineSurface", 0 ) ;
    IBSplineSurface * bs = dynamic_cast<IBSplineSurface *>(shape) ;
    bs->setDefinition(s->number1, s->number2,
		      s->order1, s->order2,
		      s->knots1, s->knots2,
		      points 
	) ;
    IParametricSurface * ps = dynamic_cast<IParametricSurface *>(shape) ;
    ps->setGrid(s->gridU(),s->gridV()) ;
    map.insert(s, shape) ;
}



void QGeometricKernel::draw(QProceduralSurface * s)
{
  int m = s->gridU() ;
  int n = s->gridV() ;
  IGLGood * result = new IGLGood(IGLGood::E_QUAD,m*n,4*(m-1)*(n-1),true) ;
  
  for(int i = 0 ; i < m ; i++) {
    float u = s->umin() + i*(s->umax()-s->umin())/(m-1) ;
    for(int j = 0 ; j < n ; j++) {
      float v = s->vmin() + j*(s->vmax()-s->vmin())/(n-1) ;
      s->eval(u, v, result->vertices+(3*m*i+3*j), result->vertices+(3*m*i+3*j+1), result->vertices+(3*m*i+3*j+2)) ;
     
    }
  }
  
  fxv<double,3> * normals = (fxv<double, 3>*)result->normals ;
  const fxv<double,3> * smp = (fxv<double, 3>*)result->vertices ;
  
  int _N[m]; std::fill(_N,_N+m,-n);  _N[0]   = 0 ;
  int _S[m]; std::fill(_S,_S+m,n);   _S[m-1] = 0 ;
  int _E[n]; std::fill(_E,_E+n,1);   _E[n-1] = 0 ;
  int _W[n]; std::fill(_W,_W+n,-1);  _W[0]   = 0 ;	
  int *N,*S,*E,*W ;
  
  for(N = _N, S = _S; N != _N + m; N++, S++)
    for(E = _E, W = _W; E != _E + n; E++, W++,  normals++, smp ++) {
      fxv<double,3> du,dv;
      sub(du,*(smp+*S),*(smp+*N));
      //  scmul(du,9.0/2.0);                                                                                                                                                                        
      sub(dv,*(smp+*E),*(smp+*W));
      //  scmul(dv,9.0/2.0);                                                                                                                                                                        
      crossprod(*normals,du,dv);
      div(*normals,sqrt(dotprod(*normals,*normals)));
    }
  // // ---------------------------------------------------------------------------------------------------
  
  // cout << "filled normals" << endl ;
  
  int c = 0 ;
  for(int i = 0 ; i < m-1; i ++)
    for(int j = 0 ; j < n-1; j ++, c += 4) {
      int a = i*n+j ;
      result->indexes[c] = a ;
      result->indexes[c+1] = a + n ;
      result->indexes[c+2] = a + n + 1 ;
      result->indexes[c+3] = a + 1 ;
    }
  // cout << "filled indexes" << endl ;
  gldraw(result) ;
}


void QGeometricKernel::create(QBSplineVolume * bv)
{
  // SplineVolume * sv=QBSplineVolumetoGo(bv); 

SplineVolume * sv=QBSplineVolumetoGo(bv);

  const int seg =bv->seg-1;

  double stepu=(sv->endparam(0)-sv->startparam(0))/(seg);
  double stepv=(sv->endparam(1)-sv->startparam(1))/(seg);
  double stepw=(sv->endparam(2)-sv->startparam(2))/(seg);

  //filled vertices and colors


  for(int k = 0; k < seg; k++) {
   float w = sv->startparam(2) +k*stepw ;

     for(int i = 0 ; i < seg; i++) {
       float u = sv->startparam(0) +i*stepu;
       for(int j = 0 ; j < seg ; j++) {
     float v = sv->startparam(1) +j*stepv ;

     SplineSurface *s1=sv->constParamSurface(w, 2);
     SplineSurface *s2=sv->constParamSurface(u+stepu, 0);
     SplineSurface *s3=sv->constParamSurface(v+stepv, 1);
     SplineSurface *s4=sv->constParamSurface(u, 0);
     SplineSurface *s5=sv->constParamSurface(v, 1);
     SplineSurface *s6=sv->constParamSurface(w+stepw, 2);

     Go::Point pt1;
     sv->point(pt1,u,v,w);
     Go::Point normalpt1w;
     s1->normal(normalpt1w,u,v);
     Go::Point normalpt1u;
     s3->normal(normalpt1u,v,w);
     Go::Point normalpt1v;
     s5->normal(normalpt1v,u,w);

     Go::Point pt2;
     sv->point(pt2,u+stepu,v,w);
     Go::Point normalpt2w;
     s1->normal(normalpt2w,u+stepu,v);
     Go::Point normalpt2u;
     s2->normal(normalpt2u,v,w);
     Go::Point normalpt2v;
     s5->normal(normalpt2v,u+stepu,w);



     Go::Point pt3;
     sv->point(pt3,u+stepu,v+stepv,w);
     Go::Point normalpt3w;
     s1->normal(normalpt3w,u+stepu,v+stepv);
     Go::Point normalpt3u;
     s2->normal(normalpt3u,v+stepv,w);
     Go::Point normalpt3v;
     s3->normal(normalpt3v,u+stepu,w);

     Go::Point pt4;
     sv->point(pt4,u,v+stepv,w);
     Go::Point normalpt4w;
     s1->normal(normalpt4w,u,v+stepv);
     Go::Point normalpt4u;
     s4->normal(normalpt4u,v+stepv,w);
     Go::Point normalpt4v;
     s3->normal(normalpt4v,u,w);

     Go::Point pt5;
     sv->point(pt5,u,v,w+stepw);
     Go::Point normalpt5w;
     s6->normal(normalpt5w,u,v);
     Go::Point normalpt5u;
     s4->normal(normalpt5u,v,w+stepw);
     Go::Point normalpt5v;
     s5->normal(normalpt5v,u,w+stepw);


     Go::Point pt6;
     sv->point(pt6,u+stepu,v,w+stepw);
     Go::Point normalpt6w;
     s6->normal(normalpt6w,u+stepu,v);
     Go::Point normalpt6u;
     s2->normal(normalpt6u,v,w+stepw);
     Go::Point normalpt6v;
     s5->normal(normalpt6v,u+stepu,w+stepw);

     Go::Point pt7;
     sv->point(pt7,u+stepu,v+stepv,w+stepw);
     Go::Point normalpt7w;
     s6->normal(normalpt7w,u+stepu,v+stepv);
     Go::Point normalpt7u;
     s2->normal(normalpt7u,v+stepv,w+stepw);
     Go::Point normalpt7v;
     s3->normal(normalpt7v,u+stepu,w+stepw);




     Go::Point pt8;
     sv->point(pt8,u,v+stepv,w+stepw);
     Go::Point normalpt8w;
     s6->normal(normalpt8w,u,v+stepv);
     Go::Point normalpt8u;
     s4->normal(normalpt8u,v+stepv,w+stepw);
     Go::Point normalpt8v;
     s3->normal(normalpt8v,u,w+stepw);


     //fill vertices



     bv->vertices[72*seg*seg*i+72*seg*j+72*k]=pt3[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+1]=pt3[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+2]=pt3[2];


     bv->vertices[72*seg*seg*i+72*seg*j+72*k+3]=pt4[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+4]=pt4[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+5]=pt4[2];

     bv->vertices[72*seg*seg*i+72*seg*j+72*k+6]=pt1[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+7]=pt1[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+8]=pt1[2];

     bv->vertices[72*seg*seg*i+72*seg*j+72*k+9]=pt2[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+10]=pt2[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+11]=pt2[2];

     bv->vertices[72*seg*seg*i+72*seg*j+72*k+12]=pt3[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+13]=pt3[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+14]=pt3[2];

     bv->vertices[72*seg*seg*i+72*seg*j+72*k+15]=pt2[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+16]=pt2[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+17]=pt2[2];


     bv->vertices[72*seg*seg*i+72*seg*j+72*k+18]=pt6[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+19]=pt6[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+20]=pt6[2];

     bv->vertices[72*seg*seg*i+72*seg*j+72*k+21]=pt7[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+22]=pt7[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+23]=pt7[2];

     bv->vertices[72*seg*seg*i+72*seg*j+72*k+24]=pt3[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+25]=pt3[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+26]=pt3[2];


     bv->vertices[72*seg*seg*i+72*seg*j+72*k+27]=pt7[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+28]=pt7[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+29]=pt7[2];


     bv->vertices[72*seg*seg*i+72*seg*j+72*k+30]=pt8[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+31]=pt8[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+32]=pt8[2];

     bv->vertices[72*seg*seg*i+72*seg*j+72*k+33]=pt4[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+34]=pt4[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+35]=pt4[2];

     bv->vertices[72*seg*seg*i+72*seg*j+72*k+36]=pt4[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+37]=pt4[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+38]=pt4[2];


     bv->vertices[72*seg*seg*i+72*seg*j+72*k+39]=pt8[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+40]=pt8[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+41]=pt8[2];


     bv->vertices[72*seg*seg*i+72*seg*j+72*k+42]=pt5[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+43]=pt5[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+44]=pt5[2];

     bv->vertices[72*seg*seg*i+72*seg*j+72*k+45]=pt1[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+46]=pt1[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+47]=pt1[2];

     bv->vertices[72*seg*seg*i+72*seg*j+72*k+48]=pt5[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+49]=pt5[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+50]=pt5[2];

     bv->vertices[72*seg*seg*i+72*seg*j+72*k+51]=pt6[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+52]=pt6[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+53]=pt6[2];

     bv->vertices[72*seg*seg*i+72*seg*j+72*k+54]=pt2[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+55]=pt2[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+56]=pt2[2];

     bv->vertices[72*seg*seg*i+72*seg*j+72*k+57]=pt1[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+58]=pt1[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+59]=pt1[2];

     bv->vertices[72*seg*seg*i+72*seg*j+72*k+60]=pt6[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+61]=pt6[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+62]=pt6[2];


     bv->vertices[72*seg*seg*i+72*seg*j+72*k+63]=pt5[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+64]=pt5[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+65]=pt5[2];


     bv->vertices[72*seg*seg*i+72*seg*j+72*k+66]=pt8[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+67]=pt8[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+68]=pt8[2];

     bv->vertices[72*seg*seg*i+72*seg*j+72*k+69]=pt7[0];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+70]=pt7[1];
     bv->vertices[72*seg*seg*i+72*seg*j+72*k+71]=pt7[2];

     //fill normals


     bv->normals[72*seg*seg*i+72*seg*j+72*k]=normalpt3w[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+1]=normalpt3w[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+2]=normalpt3w[2];


     bv->normals[72*seg*seg*i+72*seg*j+72*k+3]=normalpt4w[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+4]=normalpt4w[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+5]=normalpt4w[2];

     bv->normals[72*seg*seg*i+72*seg*j+72*k+6]=normalpt1w[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+7]=normalpt1w[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+8]=normalpt1w[2];

     bv->normals[72*seg*seg*i+72*seg*j+72*k+9]=normalpt2w[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+10]=normalpt2w[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+11]=normalpt2w[2];

     bv->normals[72*seg*seg*i+72*seg*j+72*k+12]=normalpt3u[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+13]=normalpt3u[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+14]=normalpt3u[2];

     bv->normals[72*seg*seg*i+72*seg*j+72*k+15]=normalpt2u[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+16]=normalpt2u[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+17]=normalpt2u[2];


     bv->normals[72*seg*seg*i+72*seg*j+72*k+18]=normalpt6u[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+19]=normalpt6u[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+20]=normalpt6u[2];

     bv->normals[72*seg*seg*i+72*seg*j+72*k+21]=normalpt7u[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+22]=normalpt7u[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+23]=normalpt7u[2];

     bv->normals[72*seg*seg*i+72*seg*j+72*k+24]=normalpt3v[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+25]=normalpt3v[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+26]=normalpt3v[2];


     bv->normals[72*seg*seg*i+72*seg*j+72*k+27]=normalpt7v[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+28]=normalpt7v[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+29]=normalpt7v[2];


     bv->normals[72*seg*seg*i+72*seg*j+72*k+30]=normalpt8v[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+31]=normalpt8v[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+32]=normalpt8v[2];

     bv->normals[72*seg*seg*i+72*seg*j+72*k+33]=normalpt4v[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+34]=normalpt4v[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+35]=normalpt4v[2];

     bv->normals[72*seg*seg*i+72*seg*j+72*k+36]=normalpt4u[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+37]=normalpt4u[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+38]=normalpt4u[2];


     bv->normals[72*seg*seg*i+72*seg*j+72*k+39]=normalpt8u[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+40]=normalpt8u[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+41]=normalpt8u[2];


     bv->normals[72*seg*seg*i+72*seg*j+72*k+42]=normalpt5u[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+43]=normalpt5u[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+44]=normalpt5u[2];

     bv->normals[72*seg*seg*i+72*seg*j+72*k+45]=normalpt1u[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+46]=normalpt1u[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+47]=normalpt1u[2];

     bv->normals[72*seg*seg*i+72*seg*j+72*k+48]=normalpt5v[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+49]=normalpt5v[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+50]=normalpt5v[2];

     bv->normals[72*seg*seg*i+72*seg*j+72*k+51]=normalpt6v[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+52]=normalpt6v[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+53]=normalpt6v[2];

     bv->normals[72*seg*seg*i+72*seg*j+72*k+54]=normalpt2v[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+55]=normalpt2v[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+56]=normalpt2v[2];

     bv->normals[72*seg*seg*i+72*seg*j+72*k+57]=normalpt1v[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+58]=normalpt1v[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+59]=normalpt1v[2];

     bv->normals[72*seg*seg*i+72*seg*j+72*k+60]=normalpt6w[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+61]=normalpt6w[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+62]=normalpt6w[2];


     bv->normals[72*seg*seg*i+72*seg*j+72*k+63]=normalpt5w[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+64]=normalpt5w[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+65]=normalpt5w[2];


     bv->normals[72*seg*seg*i+72*seg*j+72*k+66]=normalpt8w[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+67]=normalpt8w[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+68]=normalpt8w[2];

     bv->normals[72*seg*seg*i+72*seg*j+72*k+69]=normalpt7w[0];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+70]=normalpt7w[1];
     bv->normals[72*seg*seg*i+72*seg*j+72*k+71]=normalpt7w[2];


       }
     }
  }
} 


void QGeometricKernel::draw(QBSplineVolume * bv)
{


/*

     SplineVolume * sv=QBSplineVolumetoGo(sv1);


     const int seg =20;

     IGLGood * bv = new IGLGood(IGLGood::E_QUAD,seg*seg*seg,3*2*4*seg*seg*seg,true) ;

     //filled normals and colors


     for(int k = 0; k < seg ; k++) {
      float w = sv->startparam(2) +k*(sv->endparam(2)-sv->startparam(2))/(seg-1) ;

        for(int i = 0 ; i < seg; i++) {
          float u = sv->startparam(0) +i*(sv->endparam(0)-sv->startparam(0))/(seg-1) ;
          for(int j = 0 ; j < seg ; j++) {
        float v = sv->startparam(1) +j*(sv->endparam(1)-sv->startparam(1))/(seg-1) ;

        Go::Point pt1;
        sv->point(pt1,u,v,w);
            std::vector<Point> p(4, Point(sv->dimension()));

            sv->point(p, u, v, w, 1);

        bv->vertices[3*seg*seg*i+3*seg*j+3*k]=pt1[0];
        bv->vertices[3*seg*seg*i+3*seg*j+3*k+1]=pt1[1];
        bv->vertices[3*seg*seg*i+3*seg*j+3*k+2]=pt1[2];
          }
        }
     }


     int c = 0 ;
     for(int k = 0 ; k < seg-1; k ++)
       for(int i = 0 ; i < seg-1; i ++)
         for(int j = 0 ; j < seg-1; j ++, c += 24) {
            int a = k*seg*seg+i*seg+j ;
        bv->indexes[c] = a ;
        bv->indexes[c+1] = a + seg ;
        bv->indexes[c+2] = a + seg + 1 ;
        bv->indexes[c+3] = a + 1 ;

        bv->indexes[c+4] = a ;
        bv->indexes[c+5] = a + seg ;
        bv->indexes[c+6] = seg*seg+a+ seg  ;
        bv->indexes[c+7] = seg*seg+a  ;

        bv->indexes[c+8] = a+1 ;
        bv->indexes[c+9] = a + seg+1 ;
        bv->indexes[c+10] = seg*seg+a+ seg+1  ;
        bv->indexes[c+11] = seg*seg+a+1  ;

        bv->indexes[c+12] = seg*seg+a  ;
        bv->indexes[c+13] = seg*seg+a+seg ;
        bv->indexes[c+14] = seg*seg+a+ seg+1  ;
        bv->indexes[c+15] = seg*seg+a+1  ;

        bv->indexes[c+16] = a  ;
        bv->indexes[c+17] = a+1 ;
        bv->indexes[c+18] = seg*seg+a+1  ;
        bv->indexes[c+19] = seg*seg+a  ;

        bv->indexes[c+20] = seg*seg+a+seg ;
        bv->indexes[c+21] = a + seg ;
        bv->indexes[c+22] = a + seg + 1 ;
        bv->indexes[c+23] = seg*seg+a+seg + 1 ;

         }
     // cout << "filled indexes" << endl ;

     glEnableClientState(GL_VERTEX_ARRAY) ;
     glVertexPointer(3, GL_DOUBLE, 0, bv->vertices) ;

     //glEnableClientState(GL_COLOR_ARRAY) ;
     //glColorPointer(3,GL_FLOAT, 0, result->colors) ;

     //glEnableClientState(GL_NORMAL_ARRAY) ;
     //glNormalPointer(GL_DOUBLE, 0, result->normals) ;

     glDisable(GL_LIGHTING) ;
     glDrawElements(GL_QUADS, bv->nbi, GL_UNSIGNED_INT, bv->indexes) ;
     glEnable(GL_LIGHTING) ;
     //glEnable(GL_COLOR_MATERIAL);

     glDisableClientState(GL_VERTEX_ARRAY) ;
     //glDisableClientState(GL_COLOR_ARRAY) ;
     //glDisableClientState(GL_NORMAL_ARRAY) ;
*/

    glEnableClientState(GL_VERTEX_ARRAY) ;
    glVertexPointer(3, GL_DOUBLE, 0, bv->vertices) ;
    glEnableClientState(GL_NORMAL_ARRAY) ;
    glNormalPointer(GL_DOUBLE, 0, bv->normals) ;
    

    
    //glPushAttrib(GL_ENABLE_BIT);
    
    //glEnable(GL_BLEND) ;
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
    //glColor4f(1.0,0.5,0.1,0.9);
    glEnable(GL_POINT_SMOOTH) ;

    //glDisable(GL_LIGHTING) ;
    //glDrawElements(GL_QUADS, bv->nbi, GL_UNSIGNED_INT, bv->indexes) ;
    //glDisableClientState(GL_VERTEX_ARRAY) ;

    glDrawArrays(GL_QUADS, 0, 24*(bv->seg-1)*(bv->seg-1)*(bv->seg-1));
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING) ;
    
    glEnable(GL_LINE_SMOOTH) ;
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glColor3f(0.0,0.5,0.0);

    //glEnableClientState(GL_VERTEX_ARRAY) ;
    //glVertexPointer(3, GL_DOUBLE, 0, bv->vertices) ;

    glLineWidth(0.001);

    glDisable(GL_LIGHTING) ;
    // glDrawElements(GL_QUADS, bv->nbi, GL_UNSIGNED_INT,  bv->indexes) ;
    glDrawArrays(GL_LINES, 0, 24*(bv->seg-1)*(bv->seg-1)*(bv->seg-1));
    // glDisableClientState(GL_VERTEX_ARRAY) ;

    glEnable(GL_LIGHTING) ;
    
    //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE) ;

    glDisableClientState(GL_VERTEX_ARRAY) ;
    glDisableClientState(GL_NORMAL_ARRAY) ;

   //glBindBuffer(GL_ARRAY_BUFFER,0);
   // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
   // glDisableClientState(GL_VERTEX_ARRAY) ;


}

void QGeometricKernel::create(QBSplineVolume4f * bv)
{
  // SplineVolume * sv=QBSplineVolumetoGo(bv); 
} 


void QGeometricKernel::draw(QBSplineVolume4f * bv)
{
  SplineVolume * sv=QBSplineVolume4ftoGo(bv);
 
  hsv_colormap.set_value_range(bv->Valmax, bv->Valmin);
    
  float mc[3];

  const int seg =25;
  
  int nbv=seg*seg*seg;
  int nbi=24*seg*seg*seg;
  double *vertices = new double[3*nbv];
  int *indexes = new int[nbi];
  float *colors = new float[4*nbv];

  //filled vertices and colors
  
  for(int k = 0 ; k < seg ; k++) {
   float w = sv->startparam(2) +k*(sv->endparam(2)-sv->startparam(2))/(seg-1) ;
    
     for(int i = 0 ; i < seg ; i++) {
       float u = sv->startparam(0) +i*(sv->endparam(0)-sv->startparam(0))/(seg-1) ;
       for(int j = 0 ; j < seg ; j++) {
	 float v = sv->startparam(1) +j*(sv->endparam(1)-sv->startparam(1))/(seg-1) ;
	 
	 Go::Point pt1;
	 sv->point(pt1,u,v,w);
	 getcolor(pt1[3],mc);
	 colors[4*seg*seg*i+4*seg*j+4*k]=mc[0];
	 colors[4*seg*seg*i+4*seg*j+4*k+1]=mc[1];
	 colors[4*seg*seg*i+4*seg*j+4*k+2]=mc[2]; 
	 colors[4*seg*seg*i+4*seg*j+4*k+3]=0.6; 
	 vertices[3*seg*seg*i+3*seg*j+3*k]=pt1[0];
	 vertices[3*seg*seg*i+3*seg*j+3*k+1]=pt1[1];
	 vertices[3*seg*seg*i+3*seg*j+3*k+2]=pt1[2]; 
       }
     }
  }
  
  
  int c = 0 ;
  for(int k = 0 ; k < seg-1; k ++)
    for(int i = 0 ; i < seg-1; i ++)
      for(int j = 0 ; j < seg-1; j ++, c += 24) {
	int a = k*seg*seg+i*seg+j ;
	indexes[c] = a ;
	indexes[c+1] = a + seg ;
	indexes[c+2] = a + seg + 1 ;
	indexes[c+3] = a + 1 ;
	
	indexes[c+4] = a ;
	indexes[c+5] = a + seg ;
	indexes[c+6] = seg*seg+a+ seg  ;
	indexes[c+7] = seg*seg+a  ;

	indexes[c+8] = a+1 ;
	indexes[c+9] = a + seg+1 ;
	indexes[c+10] = seg*seg+a+ seg+1  ;
	indexes[c+11] = seg*seg+a+1  ;

       	indexes[c+12] = seg*seg+a  ;
	indexes[c+13] = seg*seg+a+seg ;
	indexes[c+14] = seg*seg+a+ seg+1  ;
	indexes[c+15] = seg*seg+a+1  ;
	
	indexes[c+16] = a  ;
	indexes[c+17] = a+1 ;
	indexes[c+18] = seg*seg+a+1  ;
	indexes[c+19] = seg*seg+a  ;
	
	indexes[c+20] = seg*seg+a+seg ;
	indexes[c+21] = a + seg ;
	indexes[c+22] = a + seg + 1 ;
	indexes[c+23] = seg*seg+a+seg + 1 ;
       
      }

  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND) ; 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
  glEnable(GL_POINT_SMOOTH) ;
 
  glEnableClientState(GL_COLOR_ARRAY) ;   
  glColorPointer(4,GL_FLOAT, 0, colors) ; 

  glEnableClientState(GL_VERTEX_ARRAY) ;
  glVertexPointer(3, GL_DOUBLE, 0, vertices) ;
  
  glDrawElements(GL_QUADS, nbi, GL_UNSIGNED_INT,  indexes) ;
  glEnable(GL_LIGHTING) ;
  glEnable(GL_COLOR_MATERIAL);
 
  glDisableClientState(GL_VERTEX_ARRAY) ;
  glDisableClientState(GL_COLOR_ARRAY) ;

  delete indexes;
  delete vertices;
  delete colors;	
	
}


float VolumeSolution(double a, double b, double c)
{
  float exact=sin(3.14156*a/3) * sin(3.14156*b/3)*sin(3.14156*c/3);

  return exact; 
}


void QGeometricKernel::draw_exactV(QExactSolutionVolume * bv)
{
  SplineVolume * sv=QBSplineVolumetoGo(bv->vol);
 
  hsv_colormap.set_value_range(bv->Valmax, bv->Valmin);
  
  float mc[3];
  float solution; 

  const int seg =25;
  
  int nbv=seg*seg*seg;
  int nbi=24*seg*seg*seg;
  double *vertices = new double[3*nbv];
  int *indexes = new int[nbi];
  float *colors = new float[4*nbv];

  //filled vertices and colors
  
  for(int k = 0 ; k < seg ; k++) {
   float w = sv->startparam(2) +k*(sv->endparam(2)-sv->startparam(2))/(seg-1) ;
    
     for(int i = 0 ; i < seg ; i++) {
       float u = sv->startparam(0) +i*(sv->endparam(0)-sv->startparam(0))/(seg-1) ;
       for(int j = 0 ; j < seg ; j++) {
	 float v = sv->startparam(1) +j*(sv->endparam(1)-sv->startparam(1))/(seg-1) ;
	 Go::Point pt1;
	 sv->point(pt1,u,v,w);
	 solution=VolumeSolution(pt1[0],pt1[1],pt1[2]);
	 getcolor(solution,mc);   

	 colors[4*seg*seg*i+4*seg*j+4*k]=mc[0];
	 colors[4*seg*seg*i+4*seg*j+4*k+1]=mc[1];
	 colors[4*seg*seg*i+4*seg*j+4*k+2]=mc[2]; 
	 colors[4*seg*seg*i+4*seg*j+4*k+3]=0.6; 
	 vertices[3*seg*seg*i+3*seg*j+3*k]=pt1[0];
	 vertices[3*seg*seg*i+3*seg*j+3*k+1]=pt1[1];
	 vertices[3*seg*seg*i+3*seg*j+3*k+2]=pt1[2]; 
       }
     }
  }
  
  
  int c = 0 ;
  for(int k = 0 ; k < seg-1; k ++)
    for(int i = 0 ; i < seg-1; i ++)
      for(int j = 0 ; j < seg-1; j ++, c += 24) {
	int a = k*seg*seg+i*seg+j ;
	indexes[c] = a ;
	indexes[c+1] = a + seg ;
	indexes[c+2] = a + seg + 1 ;
	indexes[c+3] = a + 1 ;
	
	indexes[c+4] = a ;
	indexes[c+5] = a + seg ;
	indexes[c+6] = seg*seg+a+ seg  ;
	indexes[c+7] = seg*seg+a  ;

	indexes[c+8] = a+1 ;
	indexes[c+9] = a + seg+1 ;
	indexes[c+10] = seg*seg+a+ seg+1  ;
	indexes[c+11] = seg*seg+a+1  ;

       	indexes[c+12] = seg*seg+a  ;
	indexes[c+13] = seg*seg+a+seg ;
	indexes[c+14] = seg*seg+a+ seg+1  ;
	indexes[c+15] = seg*seg+a+1  ;
	
	indexes[c+16] = a  ;
	indexes[c+17] = a+1 ;
	indexes[c+18] = seg*seg+a+1  ;
	indexes[c+19] = seg*seg+a  ;
	
	indexes[c+20] = seg*seg+a+seg ;
	indexes[c+21] = a + seg ;
	indexes[c+22] = a + seg + 1 ;
	indexes[c+23] = seg*seg+a+seg + 1 ;
       
      }
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnableClientState(GL_COLOR_ARRAY) ;   
  glColorPointer(4,GL_FLOAT, 0, colors) ; 

  glEnableClientState(GL_VERTEX_ARRAY) ;
  glVertexPointer(3, GL_DOUBLE, 0, vertices) ;
  
  glDisable(GL_LIGHTING) ;
  glDrawElements(GL_QUADS, nbi, GL_UNSIGNED_INT,  indexes) ;
  glEnable(GL_LIGHTING) ;
  glEnable(GL_COLOR_MATERIAL);
 
  glDisableClientState(GL_VERTEX_ARRAY) ;
  glDisableClientState(GL_COLOR_ARRAY) ;


  delete indexes;
  delete vertices;
  delete colors;
 
}



QPoint3f * QGeometricKernel::eval(QAxel::QRationalCurve * c, float u)
{
    IShape * shape = map.value((QAxelObject *)c) ;
    IRationalCurve * rc = dynamic_cast<IRationalCurve *>(shape) ;
    double p[3] ; rc->eval(p, u) ;
    return new QPoint3f(p[0], p[1], p[2]) ;
}

QPoint3f * QGeometricKernel::eval(QAxel::QRationalSurface * s, float u, float v)
{
    IShape * shape = map.value((QAxelObject *)s) ;
    IRationalSurface * rs = dynamic_cast<IRationalSurface *>(shape) ;
    double p[3] ; rs->eval(p, u, v) ;
    return new QPoint3f(p[0], p[1], p[2]) ;
}

void QGeometricKernel::eval(QAxel::QRationalSurface * s, float u, float v, float * x, float * y, float * z)
{
	IShape * shape = map.value((QAxelObject *)s) ;
    IRationalSurface * rs = dynamic_cast<IRationalSurface *>(shape) ;
    double p[3] ; rs->eval(p, u, v) ;
	*x = p[0];
	*y = p[1];
	*z = p[2];
}

void QGeometricKernel::eval(QAxel::QRationalSurface * s, float u, float v, double * x, double * y, double * z)
{
	IShape * shape = map.value((QAxelObject *)s) ;
    IRationalSurface * rs = dynamic_cast<IRationalSurface *>(shape) ;
    double p[3] ; rs->eval(p, u, v) ;
	*x = p[0];
	*y = p[1];
	*z = p[2];
}

QPoint3f * QGeometricKernel::eval(QAxel::QBSplineCurve * c, float u)
{
    IShape * shape = map.value((QAxelObject *)c) ;
    IBSplineCurve * bc = dynamic_cast<IBSplineCurve *>(shape) ;
    double p[3] ; bc->eval(p, u) ;
    return new QPoint3f(p[0], p[1], p[2]) ;
}

void QGeometricKernel::eval(QAxel::QBSplineCurve * c, float u, float * x, float * y, float * z)
{
    IShape * shape = map.value((QAxelObject *)c) ;
    IBSplineCurve * bc = dynamic_cast<IBSplineCurve *>(shape) ;
    double p[3] ; bc->eval(p, u) ;
    *x = p[0] ;
    *y = p[1] ;
    *z = p[2] ;
}

QPoint3f * QGeometricKernel::eval(QAxel::QBSplineSurface * s, float u, float v)
{

  SplineSurface * ss=QBSplineSurfacetoGo(s);
  Go::Point pt;
  ss->point(pt,u,v);
  
  return new QPoint3f(pt[0],pt[1],pt[2]);

}


void QGeometricKernel::eval(QBSplineSurface * s, float u, float v, float * x, float * y, float * z)
{
	IShape * shape = map.value((QAxelObject *)s) ;
    IBSplineSurface * bs = dynamic_cast<IBSplineSurface *>(shape) ;
    double p[3] ; bs->eval(p, u, v) ;
	*x = p[0];
	*y = p[1];
	*z = p[2];
}

void QGeometricKernel::eval(QBSplineSurface * s, float u, float v, double * x, double * y, double * z)
{
  IShape * shape = map.value((QAxelObject *)s) ;
  IBSplineSurface * bs = dynamic_cast<IBSplineSurface *>(shape) ;
  double p[3] ; bs->eval(p, u, v) ;
  *x = p[0];
  *y = p[1];
  *z = p[2];
}

QBSplineCurve * QGeometricKernel::eval(QBSplineSurface * surface, float t, int dir) 
{
  SISLSurf  * sislsurface = surfaces.value(surface) ;
  SISLCurve * sislcurve = NULL ;
  int         stat ;
  s1439(sislsurface, t, dir, &sislcurve, &stat) ;
  return toQBSplineCurve(sislcurve) ;
}


QPoint3f * QGeometricKernel::eval(QAxel::QBSplineVolume * bv, float u, float v, float w)
{
  SplineVolume * sv=QBSplineVolumetoGo(bv);
  Go::Point pt;
  sv->point(pt,u,v,w);
  return new QPoint3f(pt[0],pt[1],pt[2]);   
}


QPoint4f * QGeometricKernel::eval(QAxel::QBSplineVolume4f * bv, float u, float v, float w)
{
  SplineVolume * sv=QBSplineVolume4ftoGo(bv);
  Go::Point pt;
  sv->point(pt,u,v,w);
  return new QPoint4f(pt[0],pt[1],pt[2],pt[3]);   
}



QBSplineSurface * QGeometricKernel::eval(QAxel::QBSplineVolume * volume, float t, int dir) 
{
   SplineVolume * sv=QBSplineVolumetoGo(volume);
   SplineSurface *s=sv->constParamSurface((double)t, dir);
   return GotoQBSplineSurface(*s);
}

QBSplineVolume * QGeometricKernel::deriVolume(QBSplineVolume * bv, int ider1, int ider2, int ider3)
{
 
  SplineVolume * sv=QBSplineVolumetoGo(bv);
  SplineVolume * dsv=sv->derivVolume(ider1,ider2, ider3);
  return GotoQBSplineVolume(*dsv);
}


QBSplineVolume * QGeometricKernel::linearSweptVolume(QBSplineSurface * bs, QBSplineCurve *bc, QPoint3f* point)
{

  Go::Point pt =Point(point->x(), point->y(), point->z());

 SplineCurve * scurve=QBSplineCurvetoGo(bc);
 SplineSurface * ssurface=QBSplineSurfacetoGo(bs);
 
 SweepVolumeCreator * svc;
 SplineVolume * sweptvolume=svc->linearSweptVolume(*ssurface, *scurve, pt);

 return GotoQBSplineVolume(*sweptvolume);
}




QShape * QGeometricKernel::intersect(QAxelObject * s1, QAxelObject * s2) 
{
    IParametricSurface * ps1 = dynamic_cast<IParametricSurface *>(map[s1]) ;
    IParametricSurface * ps2 = dynamic_cast<IParametricSurface *>(map[s2]) ;
    IShape * shape = mmx::intersect(ps1, ps2) ;
    QShape * qshape = new QShape ;
    qshape->setRGB(255, 0, 0) ;
    map.insert(qshape, shape) ;
    return qshape ;
}

QShape * QGeometricKernel::selfintersect(QAxelObject * s) 
{
    IParametricSurface * ps = dynamic_cast<IParametricSurface *>(map[s]) ;
    IShape * shape = mmx::selfintersection(ps) ;
    QShape * qshape = new QShape(s) ;
    qshape->setRGB(255, 0, 0) ;
    map.insert(qshape, shape) ;
    return qshape ;
}

QList<QPiecewiseLinearCurve *> QGeometricKernel::selfIntersectionCurves(QShape * s)
{
    QList<QPiecewiseLinearCurve *> l ;
    
    IParametricShape * pshape = dynamic_cast<IParametricShape *>(map[s]) ;
    QParametricSurface *   ps = dynamic_cast<QParametricSurface *>(s->parent()) ;

    if(!pshape) {
	qCritical("shape does not seem to be a parametric shape in the virtual hierarchy") ;
	return l ;
    }

    if(!ps) {
	qCritical("shape does not seem to be a QParametricSurface") ;
	return l ;
    }

    if(!pshape->lprms()) {
	qCritical("shape does not seem to have left self intersection curves fullfilled") ;
	return l ;
    }

    if(!pshape->rprms()) {
	qCritical("shape does not seem to have right self intersection curves fullfilled") ;
	return l ;
    }

    for(int i = 0 ; i < pshape->lprms()->size ; i++) {
	QPiecewiseLinearCurve * c = new QPiecewiseLinearCurve(ps->umin(), ps->umax(), ps->vmin(), ps->vmax()) ;
	for(int j = 0 ; j < pshape->lprms()->params[i].size ; j++)
	    c->push(new QVertex(pshape->lprms()->params[i].prms[j][0], pshape->lprms()->params[i].prms[j][1], 0.0, c)) ;
	c->link() ;
//	c->setRGB(i*255/pshape->lprms()->size, 255 - i*255/pshape->lprms()->size, (128 + i*255/pshape->lprms()->size) % 255) ;
	l << c ;
    }

    for(int i = 0 ; i < pshape->rprms()->size ; i++) {
	QPiecewiseLinearCurve * c = new QPiecewiseLinearCurve(ps->umin(), ps->umax(), ps->vmin(), ps->vmax()) ;
	for(int j = 0 ; j < pshape->rprms()->params[i].size ; j++)
	    c->push(new QVertex(pshape->rprms()->params[i].prms[j][0], pshape->rprms()->params[i].prms[j][1], 0.0, c)) ;
	c->link() ;
//	c->setRGB((64+i*255/pshape->lprms()->size)%255, i*255/pshape->lprms()->size, 255 - i*255/pshape->lprms()->size) ;
	l << c ;
    }

    return l ;
}

void QGeometricKernel::draw(QAxelObject * object) 
{
    gldraw(map[object]->mesh()) ;
}

int QGeometricKernel::nindexes(QShape * s)
{
    IShape * is = dynamic_cast<IShape *>(map[s]) ;
    IGLGood * glg = is->mesh() ;
    return glg->nbi ;
}

int QGeometricKernel::nvertices(QShape * s)
{
    IShape * is = dynamic_cast<IShape *>(map[s]) ;
    IGLGood * glg = is->mesh() ;
    return glg->nbv ;
}

void QGeometricKernel::triangulate(QShape * s, int * nbi, int * indices, int * nbv, double * vertices)
{
    IShape * is = dynamic_cast<IShape *>(map[s]) ;
    IGLGood * glg = is->mesh() ;
    *nbi = glg->nbi ;
    for(int i = 0 ; i < glg->nbi ; i++) *(indices+i) = glg->indexes[i] ;
    *nbv = glg->nbv ;
    for(int i = 0 ; i < glg->nbv ; i++) for(int j = 0 ; j < 3 ; j++) *(vertices+3*i+j) = glg->vertices[3*i+j] ;
}

int QGeometricKernel::nindexes(QBSplineCurve * c)
{
    IParametricCurve * pc = dynamic_cast<IParametricCurve *>(map[c]) ;
    IGLGood * glg = pc->mesh() ;
    return glg->nbi ;
}

int QGeometricKernel::nvertices(QBSplineCurve * c)
{
    IParametricCurve * pc = dynamic_cast<IParametricCurve *>(map[c]) ;
    IGLGood * glg = pc->mesh() ;
    return glg->nbv ;
}

void QGeometricKernel::triangulate(QBSplineCurve * c, int * nbi, int * indices, int * nbv, double * vertices)
{
    IParametricCurve * pc = dynamic_cast<IParametricCurve *>(map[c]) ;
    IGLGood * glg = pc->mesh() ;
    *nbi = glg->nbi ;
    for(int i = 0 ; i < glg->nbi ; i++) *(indices+i) = glg->indexes[i] ;
    *nbv = glg->nbv ;
    for(int i = 0 ; i < glg->nbv ; i++) for(int j = 0 ; j < 3 ; j++) *(vertices+3*i+j) = glg->vertices[3*i+j] ;
}

int QGeometricKernel::nindexes(QBSplineSurface * s)
{
    IParametricSurface * ps = dynamic_cast<IParametricSurface *>(map[s]) ;
    IGLGood * glg = ps->mesh() ;
    return 6*glg->nbi/4 ;
}

int QGeometricKernel::nvertices(QBSplineSurface * s)
{
    IParametricSurface * ps = dynamic_cast<IParametricSurface *>(map[s]) ;
    IGLGood * glg = ps->mesh() ;
    return glg->nbv ;
}

void QGeometricKernel::triangulate(QBSplineSurface * s, int * nbi, int * indices, int * nbv, double * vertices, double * normals)
{
    IParametricSurface * ps = dynamic_cast<IParametricSurface *>(map[s]) ;
    IGLGood * glg = ps->triangulate() ;
    *nbi = glg->nbi ;
    for(int i = 0 ; i < glg->nbi ; i++) *(indices+i) = glg->indexes[i] ;
    *nbv = glg->nbv ;
    for(int i = 0 ; i < glg->nbv ; i++) for(int j = 0 ; j < 3 ; j++) *(vertices+3*i+j) = glg->vertices[3*i+j] ;
    for(int i = 0 ; i < glg->nbv ; i++) for(int j = 0 ; j < 3 ; j++) *( normals+3*i+j) = glg->normals[3*i+j] ;
}

QBSplineCurve * QGeometricKernel::readCurve(QFile * file)
{
    ifstream is(file->fileName().toLatin1().data()) ;
    SISLCurve * sislcurve = readGoCurve(is) ;    
    return toQBSplineCurve(sislcurve) ;
}

QBSplineSurface * QGeometricKernel::readSurface(QFile * file)
{
    ifstream is(file->fileName().toLatin1().data()) ;
    SISLSurf * sislsurface = readGoSurface(is) ;    
    return toQBSplineSurface(sislsurface) ;
}


QList<QBSplineSurface *>  QGeometricKernel::readIGESSurface(QFile * file)
{
    ifstream is(file->fileName().toLatin1().data()) ;
    IGESconverter iges;
  
    iges.readIGES(is);
    std::ofstream outfile("output");
    iges.writego(outfile);
   
    // std::vector<Go::SplineSurface*>  spline_surface= iges.readIGESSurface(is);
    std::vector<boost::shared_ptr<Go::GeomObject> > spline_surface
	= iges.getGoGeom();
    QList<QBSplineSurface *> qspline_surface; 
    for(int i=0; i<spline_surface.size(); i++)
      {   
	// Go::SplineSurface*  solsurf0= spline_surface.at(i);
	  Go::GeomObject* geomobj = spline_surface.at(i).get();
	  Go::SplineSurface* solsurf0
	      = dynamic_cast<Go::SplineSurface*>(geomobj);
	  if (!solsurf0)
	      continue;

        // Define axel surface
	QBSplineSurface  *solsurf1 = new QBSplineSurface;
	
	// define features
	solsurf1->order1 = solsurf0->order_u();
	solsurf1->order2 = solsurf0->order_v();
	solsurf1->number1 = solsurf0->numCoefs_u();
	solsurf1->number2 = solsurf0->numCoefs_v();
	
	solsurf1->uniform = false;
	solsurf1->ikind = 1;
	solsurf1->idim = 3;
	solsurf1->icopy=0;
	
	solsurf1->knots1 = const_cast<double*>(&(*(solsurf0->basis(0).begin()))) ;
	solsurf1->knots2 = const_cast<double*>(&(*(solsurf0->basis(1).begin()))) ;
	
	std::vector<double>::const_iterator coefsstart = solsurf0->coefs_begin();
	double *ecoef = const_cast<double*>(&(*coefsstart));
	
	for(int i = 0 ; i <  solsurf1->number1 * solsurf1->number2 ; i++)
	  {
	    solsurf1->push_point(new QPoint3f(ecoef[3*i+0], ecoef[3*i+1],ecoef[3*i+2]));
	  }

	solsurf1->compute();
	
        qspline_surface.push_back(solsurf1);

      }
    fprintf(stderr,"number of surface %d \n", qspline_surface.size());
    return qspline_surface; 
}
 

QList<QBSplineCurve *>  QGeometricKernel::readIGESCurve(QFile * file)
{
    ifstream is(file->fileName().toLatin1().data()) ;
    IGESconverter iges;
    // std::vector<Go::SplineCurve*>  spline_curve= iges.readIGESCurve(is);
    std::vector<boost::shared_ptr<Go::GeomObject> >  spline_curve
	= iges.getGoGeom();
    
    QList<QBSplineCurve *> qspline_curve; 
    for(int i=1; i<spline_curve.size(); i++)
      {
	// SplineCurve*  solcurv0= spline_curve.at(i);
	  Go::GeomObject* geomobj = spline_curve.at(i).get();
	  Go::SplineCurve* solcurv0
	      = dynamic_cast<Go::SplineCurve*>(geomobj);
	  if (!solcurv0)
	      continue;
         
        // define axel surface
	QBSplineCurve  *solcurv1 = new QBSplineCurve;
	
	// define features
	solcurv1->order = solcurv0->order();
        fprintf(stderr,"storage of solver parameters :\n");
	solcurv1->number = solcurv0->numCoefs();
	
	solcurv1->uniform = false;
	solcurv1->ikind = 1;
	solcurv1->idim = 3;

	solcurv1->icopy=0;
	

 
	solcurv1->knots = const_cast<double*>(&(*(solcurv0->basis().begin()))) ;
 
	std::vector<double>::const_iterator coefsstart = solcurv0->coefs_begin();
	double *ecoef = const_cast<double*>(&(*coefsstart));
	
	for(int i = 0 ; i <  solcurv1->number; i++)
	  {
	    solcurv1->push_point(new QPoint3f(ecoef[3*i+0], ecoef[3*i+1],ecoef[3*i+2]));
	  }
	solcurv1->compute();
	
        qspline_curve.push_back(solcurv1);

      }

    return qspline_curve; 
}
 

QPoint3f * QGeometricKernel::closestPoint(QBSplineCurve * curve, QPoint3f * p)
{
    // calculating closest point                                                                                                                                                                             
    SISLCurve * c = curves.value(curve) ;
    double epoint[] = { p->x(), p->y(), p->z() } ;
    double aepsco = 1.0e-15 ;
    double aepsge = 1.0e-5 ;
    double dist ;
    double gpar ;
    int jstat, temp ; // status variable
    
    s1957(c,
	  epoint,
	  3,
	  aepsco,           // computational resolution                                                                                                     
	  aepsge,           // geometry resolution                                                  
	  &gpar,
	  &dist,
	  &jstat);
    
    if (jstat < 0) {
	cout << FG_RED << "Error occured inside call to SISL routine s1957." << NOCOLOR << endl ;
    } else if (jstat > 0) {
	cout << FG_MAGENTA << "WARNING: warning occured inside call to SISL routine s1957. \n" << NOCOLOR << endl;
    }
    
    double coords[3] ;
    s1227(c, 			// we evaluate on the curve
          0,          	// calculate no derivatives
          gpar, 	    // parameter value on which to evaluate
          &temp,      	// not used for our purposes (gives parameter interval)
          coords, 		// result written here
          &jstat);
    
    QPoint3f * r = new QPoint3f(coords[0], coords[1], coords[2]) ;
    return r ;
}

QBSplineCurve * QGeometricKernel::approximateBSplineCurve(QList<QPoint3f *> points, int iopen, int ikind, int order, int dimension)
{
    int inbpnt = points.size() ;

    double epoints[3*inbpnt] ;
    for(int i=0 ; i < inbpnt ; i++) {
	epoints[3*i+0] = points[i]->x() ;
	epoints[3*i+1] = points[i]->y() ;
	epoints[3*i+2] = points[i]->z() ;
    }
    double astpar = 0.0 ;
    int    idim   = dimension ;
    int    ik     = order ;
    
    SISLCurve * result = 0 ;
    int jstat ;
    
    s1630(epoints,
	  inbpnt,
	  astpar,
	  iopen,
	  idim,
	  ik,
	  &result,
	  &jstat) ;

    QBSplineCurve * c = toQBSplineCurve(result) ;
    c->ikind = ikind ;

    return c;
}

QBSplineSurface * QGeometricKernel::approximateBSplineSurface(QList<QPoint3f *> points, int iopen1 , int iopen2 , int order1, int order2, int dimension)
{
    int inbput1 = 0, inbput2 = 0 ;
    double size = sqrt(points.size()) ;

    if(size ==(int)size && size >= order1 && size >= order2) {
	inbput1 = (int)size ;
	inbput2 = (int)size ;
    }
    else {
	QMessageBox::warning(0, "Axel","Caution : Number of Points selected must be a perfect square and its square root must be >= both order1 & order2 !!","&Ok",0) ;
	return NULL ;
    }
	
    double * epoints = new double[inbput1*inbput2*3] ;
    for(int i = 0 ; i < inbput1*inbput2 ; i++){
	epoints[3*i+0] = points[i]->x() ;
	epoints[3*i+1] = points[i]->y() ;
	epoints[3*i+2] = points[i]->z() ;
    }
    int idim = dimension ;
    SISLSurf * result = 0 ;
    int jstat ;
    
    s1620(epoints,
	  inbput1,
	  inbput2,
	  2,
	  iopen1,
	  iopen2,
	  order1,
	  order2,
	  idim,
	  &result,
	  &jstat) ;

    return toQBSplineSurface(result) ;
}

QBSplineCurve * QGeometricKernel::join(QBSplineCurve * c1, QBSplineCurve * c2)
{
    SISLCurve * sc1 = curves.value(c1) ;
    SISLCurve * sc2 = curves.value(c2) ;
    int end1 = 1 ;
    int end2 = 0 ;
    SISLCurve * newcurve ;
    int stat ;

    s1715(sc1,
	  sc2,
	  end1,
	  end2,
	  &newcurve,
	  &stat) ;

    return toQBSplineCurve(newcurve) ;
}

QBSplineCurve * QGeometricKernel::interpolate(QList<QPoint3f *> points)
{
    int num_points = points.size() ;
    double cpoints[3*num_points] ;
    for(int i=0 ; i<num_points ; i++) {
	cpoints[3*i+0] = points[i]->x() ;
	cpoints[3*i+1] = points[i]->y() ;
	cpoints[3*i+2] = points[i]->z() ;
    }
    
    int types[num_points] ;
    for(int i=0 ; i<num_points ; i++) types[i] = 1 ;
    
    SISLCurve * result = 0 ;
    double cendpar ;
    double * gpar = 0 ;
    int jnbpar, jstat ;
    
    s1356(cpoints,		// pointer to where the coordinates are stored
	  num_points,	// number of points to be interpolated
	  3,			// input dimension
	  types,		// what type of information is stored at each point (1 means ordinary point)
	  0, 			// no additional condition at start point
	  0,			// no additional condition at end point
	  1, 			// open curve
	  4, 			// order of the spline curve to be produced
	  0, 			// parameter value to be used at start of curve
	  &cendpar,		// parameter value to be used at end of curve
	  &result, 		// the resulting spline curve
	  &gpar, 		// pointer to the parameter values of the points in the curve
	  &jnbpar, 		// number of unique parameter values
	  &jstat		// status message
	) ;
    
    return toQBSplineCurve(result) ;
}

QBSplineCurve * QGeometricKernel::blend(QBSplineCurve * curve1, QBSplineCurve * curve2)
{
    SISLCurve* c1 = curves.value(curve1) ;
    SISLCurve* c2 = curves.value(curve2) ;
    
    double epsge = 1.0e-5; // geometric precision
    int blendtype = 0;     // generate polynomial segment
    int dim = 3;
    int order = 4;
    double c1_endpoint[3]; // endpoint of curve 1 (must be calculated)
    double c2_endpoint[3]; // endpoint of curve 2 (must be calculated)
    
    // The blend curve will extend from the endpoint of curve 1 to the
    // endpoint of curve 2.  As input, the SISL routine needs (approximate)
    // coordinates for these points on the curve.  We therefore preliminarly
    // need to evaluate these.  For this purpose, we use SISL routine s1227.
    
    // The end parameters of the curves' parametric domains can be found by 
    // looking at their knotvectors (pointed to by data member 'et').  
    // If the number of control points is 'n', then the knot numbered 'n'
    // would represent the end parameter (when counting from 0).  The number
    // of control points is indicated by the data member 'in'.
    double c1_endpar = c1->et[c1->in]; // end parameter of curve 1
    double c2_endpar = c2->et[c2->in]; // end parameter of curve 2
    int temp, jstat1, jstat2;
    
    // evaluating endpoint positions of both curves
    s1227(c1,          // input curve
	  0,           // evaluate position only (no derivatives)
	  c1_endpar,   // end parameter
	  &temp,       // indicates param. interval (not interesting for our purposes)
	  c1_endpoint, // this is what we want to calculate (3D position)
	  &jstat1);     // status variable (0 if everything all right)
    
    s1227(c2,          // input curve
	  0,           // evaluate position only (no derivatives)
	  c2_endpar,   // end parameter
	  &temp,       // indicates param. interval (not interesting for our purposes)
	  c2_endpoint, // this is what we want to calculate (3D position)
	  &jstat2);     // status variable (0 if everything all right)
    
    if (jstat1 < 0 || jstat2 < 0) {
	cout << FG_RED << "Error occured inside call to SISL routine s1227." << NOCOLOR << endl ;
    } else if (jstat1 > 0 || jstat2 > 0) {
	cout << FG_MAGENTA << "WARNING: warning occured inside call to SISL routine s1227." << NOCOLOR << endl ;
    }
    
    SISLCurve* blend_curve = 0;
    int jstat;
    
    s1606(c1,           // the first input curve
	  c2,           // the second input curve
	  epsge,        // geometric tolerance
	  c1_endpoint,  // endpoint of curve 1 (geometric)
	  c2_endpoint,  // endpoint of curve 2 (geometric)
	  blendtype,    // type of blend curve (circle, conic, polynomial)
	  dim,          // dimension (3D)
	  order,        // order of generated spline curve
	  &blend_curve, // the generated curve
	  &jstat);      // status message
    
    return toQBSplineCurve(blend_curve) ;
}

QBSplineCurve * QGeometricKernel::offset(QBSplineCurve * curve, Vec dir, double distance)
{
    SISLCurve* c1 = curves.value(curve) ;
    
    double offset = double(distance);                // the offset between the 'old' curve and the result.
    double epsge = 1e-5;                             // geometric tolerance 
    double direction[] = {dir.x, dir.y, dir.z};      // the direction of the offset
    int dim = 3;                                     // the dimension of the Euclidean space
    int jstat;                                       // status variable
    SISLCurve* offset_curve = 0;
    
    s1360(c1,        // the 'old' curve
	  offset,        // the offset value
	  epsge,         // geometric tolerance
	  direction,     // offset direction
	  0,             // max step length.  0 indicate the longest box side of 's1'
	  dim,           // the dimension
	  &offset_curve, // the resulting offset curve
	  &jstat);       // status variable
    
    if (jstat < 0) {
	cout << FG_RED << "Error occured inside call to SISL routine s1360." << NOCOLOR << endl ;
    } else if (jstat > 0) {
	cout << FG_MAGENTA << "WARNING: warning occured inside call to SISL routine s1360. \n" << NOCOLOR << endl ;
    }
    
    return toQBSplineCurve(offset_curve);
}

QBSplineSurface * QGeometricKernel::offset(QBSplineSurface * surface, double distance)
{
    SISLSurf *   s = surfaces.value(surface) ;
    double aoffset = double(distance)*-1 ;
    double espge   = 1e-3 ;
    double amax    = 2 ;
    int    idim    = 3 ;

    SISLSurf *   r = NULL ;
    int jstat ;

    s1365(s, aoffset, espge, amax, idim, &r, &jstat) ;

    return toQBSplineSurface(r) ;
}

QList<QPoint3f *> QGeometricKernel::intersection(QBSplineCurve * curve1, QBSplineCurve * curve2)
{
    SISLCurve * c1 = curves.value(curve1) ;
    SISLCurve * c2 = curves.value(curve2) ;

    double epsco = 1.0e-15;  // computational epsilon
    double epsge = 1.0e-6;   // geometric tolerance
    int num_int_points = 0;  // number of found intersection points
    double* intpar1 = 0;     // parameter values for the first curve in the intersections
    double* intpar2 = 0;     // parameter values for the second curve in the intersections
    int num_int_curves = 0;  // number of intersection curves
    SISLIntcurve** intcurve = 0; // pointer to array of detected intersection curves
    int jstat;               // status variable

    s1857(c1,              // first curve 
	  c2,              // second curve
	  epsco,           // computational resolution
	  epsge,           // geometry resolution
	  &num_int_points, // number of single intersection points
	  &intpar1,        // pointer to array of parameter values
	  &intpar2,        //               "
	  &num_int_curves, // number of detected intersection curves
	  &intcurve,       // pointer to array of detected intersection curves.
	  &jstat);
  
    if (jstat < 0) {
	cout << FG_RED << "Error occured inside call to SISL routine s1857." << NOCOLOR << endl ;
    } else if (jstat > 0) {
	cout << FG_MAGENTA << "WARNING: warning occured inside call to SISL routine s1857." << NOCOLOR << endl;
    }

    vector<double> point_coords_3D(3*num_int_points);
    int i;
    for (i = 0; i < num_int_points; ++i) {
	// calculating position, using curve 1
	// (we could also have used curve 2, which would give approximately
	// the same points).
	int temp;
	s1227(c1,         // we evaluate on the first curve
	      0,          // calculate no derivatives
	      intpar1[i], // parameter value on which to evaluate
	      &temp,      // not used for our purposes (gives parameter interval)
	      &point_coords_3D[3*i], // result written here
	      &jstat);
    
	if (jstat < 0) {
	    cout << FG_RED << "Error occured inside call to SISL routine s1227." << NOCOLOR << endl ;
	} else if (jstat > 0) {
	    cout << FG_MAGENTA << "WARNING: warning occured inside call to SISL routine s1227. \n" << NOCOLOR <<endl;
	}
    }
  
    QList<QPoint3f *> intersections ;
    for(int i=0 ; i < num_int_points ; i++)
	intersections << new QPoint3f(point_coords_3D[3*i+0],point_coords_3D[3*i+1], point_coords_3D[3*i+2]) ;

    return intersections ;
}

QList<QPoint3f *> QGeometricKernel::intersection(QBSplineCurve * curve, QCell * cell)
{
    QPoint3f * tlCorner = new QPoint3f(cell->xmin(), cell->ymax(), 0.0) ;
    QPoint3f * trCorner = new QPoint3f(cell->xmax(), cell->ymax(), 0.0) ;
    QPoint3f * blCorner = new QPoint3f(cell->xmin(), cell->ymin(), 0.0) ;
    QPoint3f * brCorner = new QPoint3f(cell->xmax(), cell->ymin(), 0.0) ;

    double tnormal[3] = {  0.0,  1.0, 0.0 } ; 
    double rnormal[3] = {  1.0,  0.0, 0.0 } ; 
    double bnormal[3] = {  0.0, -1.0, 0.0 } ; 
    double lnormal[3] = { -1.0,  0.0, 0.0 } ; 

    QList<QPoint3f *> l ;
    foreach(QPoint3f * p, intersection(curve, qMakePair(tlCorner, trCorner), tnormal)) if(cell->contains(p, false)) { p->sety(tlCorner->y()) ; l << p ; }
    foreach(QPoint3f * p, intersection(curve, qMakePair(trCorner, brCorner), rnormal)) if(cell->contains(p, false)) { p->setx(trCorner->x()) ; l << p ; }
    foreach(QPoint3f * p, intersection(curve, qMakePair(brCorner, blCorner), bnormal)) if(cell->contains(p, false)) { p->sety(brCorner->y()) ; l << p ; }
    foreach(QPoint3f * p, intersection(curve, qMakePair(blCorner, tlCorner), lnormal)) if(cell->contains(p, false)) { p->setx(blCorner->x()) ; l << p ; }

    for(int i = 0 ; i < l.size() ; i++)
	for(int j = i+1 ; j < l.size() ; j++)
	    if(l.at(j)->equals(l.at(i), 1e-4)) {
		l.removeAt(j) ;
		j = i+1 ;
	    }

    return l ;
}

QList<QPoint3f *> QGeometricKernel::intersection(QBSplineCurve * curve, QPair<QPoint3f *, QPoint3f *> line, double * normal)
{
    QPoint3f * a = line.first  ;

    SISLCurve * sislcurve = curves.value(curve) ;
    double   point[3] = { a->x(), a->y(), 0.0 } ;
    int       dim     = 3 ;
    double  epsco     = 1e-3 ; // Computational resoluation, not used by sisl
    double  epsge     = 1e-6 ; // Geometry resolution

    int             numintpts ; // number of single intersection points
    double *           intpar ; // values of single intersection points
    int              numintcu ; // number of intersection curves
    SISLIntcurve ** intcurves ; // values of intersection curves
    int                  stat ;

    s1850(sislcurve,
	  point,
	  normal,
	  dim,
	  epsco,
	  epsge,
	  &numintpts,
	  &intpar,
	  &numintcu,
	  &intcurves,
	  &stat) ;

    if (stat < 0) {
	cout << FG_RED << "Error occured inside call to SISL routine s1850." << NOCOLOR << endl ;
    } else if (stat > 0) {
	cout << FG_MAGENTA << "Warning occured inside call to SISL routine s1850. \n" << NOCOLOR << endl;
    }

    // evaluating intersection points
    vector<double> point_coords_3D(3*numintpts) ;
    for (int i = 0; i < numintpts; ++i) {
       	int temp;
	s1227(sislcurve,
	      0,          // calculate no derivatives
	      intpar[i],  // parameter value on which to evaluate
	      &temp,      // not used for our purposes (gives parameter interval)
	      &point_coords_3D[3*i], // result written here
	      &stat) ;
    
	if (stat < 0) {
	    cout << FG_RED << "Error occured inside call to SISL routine s1227." << NOCOLOR << endl ;
	} else if (stat > 0) {
	    cout << FG_MAGENTA << "WARNING: warning occured inside call to SISL routine s1227. \n" << NOCOLOR << endl;
	}
    }
  
    QList<QPoint3f *> intersections ;
    for(int i=0 ; i < numintpts ; i++)
	intersections << new QPoint3f(point_coords_3D[3*i+0],point_coords_3D[3*i+1], 0.0) ;
    
    return intersections ;
}

QList<QPoint3f *> QGeometricKernel::intersection(QBSplineCurve * curve, QPlane * plane)
{
    cout << FG_RED << "QGeometricKernel::intersection(QBSplineCurve * curve, QPlane * plane)" << NOCOLOR << " WILL NOT WORK, FIND A POINT INCIDENT TO THE PLANE" << endl ;

    SISLCurve * sislcurve = curves.value(curve) ;
    double   point[3] = { 0.0, 0.0, 0.0 } ;
    double  normal[3] = { plane->a(), plane->b(), plane->c() } ;
    int       dim     = 3 ;
    double  epsco     = 1e-3 ; // Computational resoluation, not used by sisl
    double  epsge     = 1e-6 ; // Geometry resolution

    int             numintpts ; // number of single intersection points
    double *           intpar ; // values of single intersection points
    int              numintcu ; // number of intersection curves
    SISLIntcurve ** intcurves ; // values of intersection curves
    int                  stat ;

    s1850(sislcurve,
	  point,
	  normal,
	  dim,
	  epsco,
	  epsge,
	  &numintpts,
	  &intpar,
	  &numintcu,
	  &intcurves,
	  &stat) ;

    if (stat < 0) {
	cout << FG_RED << "Error occured inside call to SISL routine s1850." << NOCOLOR << endl ;
    } else if (stat > 0) {
	cout << FG_MAGENTA << "WARNING: warning occured inside call to SISL routine s1850. \n" << NOCOLOR << endl;
    }

    // evaluating intersection points
    vector<double> point_coords_3D(3*numintpts) ;
    for (int i = 0; i < numintpts; ++i) {
       	int temp;
	s1227(sislcurve,
	      0,          // calculate no derivatives
	      intpar[i],  // parameter value on which to evaluate
	      &temp,      // not used for our purposes (gives parameter interval)
	      &point_coords_3D[3*i], // result written here
	      &stat) ;
    
	if (stat < 0) {
	    cout << FG_RED << "Error occured inside call to SISL routine s1227." << NOCOLOR << endl ;
	} else if (stat > 0) {
	    cout << FG_MAGENTA << "WARNING: warning occured inside call to SISL routine s1227. \n" << NOCOLOR << endl;
	}
    }
  
    QList<QPoint3f *> intersections ;
    for(int i=0 ; i < numintpts ; i++)
	intersections << new QPoint3f(point_coords_3D[3*i+0],point_coords_3D[3*i+1], point_coords_3D[3*i+2]) ;
    
    return intersections ;
}

QList<QPoint3f *> QGeometricKernel::intersectionX(QBSplineCurve * c, float y, float xmin, float xmax)
{
    QList<QPoint3f *> l ;
    double normal[3] = { 0.0, 1.0, 0.0 } ;
    QPoint3f pmin(xmin, y, 0.0) ;
    QPoint3f pmax(xmax, y, 0.0) ;
    foreach(QPoint3f * p, intersection(c, qMakePair(&pmin, &pmax), normal))
	if(p->x() >= xmin && p->x() <= xmax)
	    l << p ;
    return l ;
}

QList<QPoint3f *> QGeometricKernel::intersectionY(QBSplineCurve * c, float x, float ymin, float ymax)
{
    QList<QPoint3f *> l ;
    double normal[3] = { 1.0, 0.0, 0.0 } ;
    QPoint3f pmin(x, ymin, 0.0) ;
    QPoint3f pmax(x, ymax, 0.0) ;
    foreach(QPoint3f * p, intersection(c, qMakePair(&pmin, &pmax), normal))
	if(p->y() >= ymin && p->y() <= ymax)
	    l << p ;
    return l ;
}

QList<QPoint3f *> QGeometricKernel::intersection(QPiecewiseLinearCurve * c1, QPiecewiseLinearCurve * c2) 
{
    QList<QPoint3f *> l ;
    foreach(QEdge * e1, c1->edges()) {
	foreach(QEdge * e2, c2->edges()) {
	    float x, y ;
	    if(ToolManager::instance()->segmentIntersection(e1->source()->x(), e1->source()->y(), e1->destination()->x(), e1->destination()->y(), 
						e2->source()->x(), e2->source()->y(), e2->destination()->x(), e2->destination()->y(),
							    &x, &y))
		l << new QPoint3f(x, y, 0.0) ;
	}
    }

    return l ;
}

QList<QPoint3f *> QGeometricKernel::intersection(QPiecewiseLinearCurve * c, QCell * cell)
{
    QList<QPoint3f *> l ;

    QPoint3f * tlCorner = new QPoint3f(cell->xmin(), cell->ymax(), 0.0) ;
    QPoint3f * trCorner = new QPoint3f(cell->xmax(), cell->ymax(), 0.0) ;
    QPoint3f * blCorner = new QPoint3f(cell->xmin(), cell->ymin(), 0.0) ;
    QPoint3f * brCorner = new QPoint3f(cell->xmax(), cell->ymin(), 0.0) ;

    foreach(QEdge * e, c->edges()) {
	float x, y ;

	if(ToolManager::instance()->segmentIntersection(e->source()->x(), e->source()->y(), e->destination()->x(), e->destination()->y(), tlCorner->x(), tlCorner->y(), trCorner->x(), trCorner->y(), &x, &y)) {
	    QPoint3f * p = new QPoint3f(x, y, 0.0) ;
	    if(!contains(l, p)) l << p ;
	}

	if(ToolManager::instance()->segmentIntersection(e->source()->x(), e->source()->y(), e->destination()->x(), e->destination()->y(), trCorner->x(), trCorner->y(), brCorner->x(), brCorner->y(), &x, &y)) {
	    QPoint3f * p = new QPoint3f(x, y, 0.0) ;
	    if(!contains(l, p)) l << p ;
	}

	if(ToolManager::instance()->segmentIntersection(e->source()->x(), e->source()->y(), e->destination()->x(), e->destination()->y(), brCorner->x(), brCorner->y(), blCorner->x(), blCorner->y(), &x, &y)) {
	    QPoint3f * p = new QPoint3f(x, y, 0.0) ;
	    if(!contains(l, p)) l << p ;
	}
	
	if(ToolManager::instance()->segmentIntersection(e->source()->x(), e->source()->y(), e->destination()->x(), e->destination()->y(), blCorner->x(), blCorner->y(), tlCorner->x(), tlCorner->y(), &x, &y)) {
	    QPoint3f * p = new QPoint3f(x, y, 0.0) ;
	    if(!contains(l, p)) l << p ;
	}
    }

    return l ;
}

QBoundingBox * QGeometricKernel::bounding(QBSplineCurve * curve)
{
    SISLCurve * sislcurve = curves.value(curve) ;
    double    * emin      = NULL ; // Bottom-left corner of the box
    double    * emax      = NULL ; // Top-right   corner of the box
    int         jstat     = 0 ;

    s1988(sislcurve,
	  &emax,
	  &emin,
	  &jstat) ;

    if (jstat < 0) {
	cout << FG_RED << "Error occured inside call to SISL routine s1988: Computing the bounding box of a curve." << NOCOLOR << endl ;
    } else if (jstat > 0) {
	cout << FG_MAGENTA << "WARNING: warning occured inside call to SISL routine s1988: Computing the bounding box of a curve." << NOCOLOR << endl;
    }

    return new QBoundingBox(emin[0], emax[0], emin[1], emax[1], emin[2], emax[2]) ;
}

QBoundingBox * QGeometricKernel::bounding(QBSplineSurface * surface)
{
    SISLSurf * sislsurface = surfaces.value(surface) ;
    double   * emin        = NULL ; // Bottom-left corner of the box
    double   * emax        = NULL ; // Top-right   corner of the box
    int        jstat       = 0 ;

    s1989(sislsurface,
	  &emax,
	  &emin,
	  &jstat) ;

    if (jstat < 0) {
	cout << FG_RED << "Error occured inside call to SISL routine s1989: Computing the bounding box of a surface." << NOCOLOR << endl ;
    } else if (jstat > 0) {
	cout << FG_MAGENTA << "WARNING: warning occured inside call to SISL routine s1989: Computing the bounding box of a surface." << NOCOLOR << endl;
    }

    return new QBoundingBox(emin[0], emax[0], emin[1], emax[1], emin[2], emax[2]) ;
}

QBSplineCurve * QGeometricKernel::pick(QBSplineCurve * curve, float begin, float end)
{
    SISLCurve * sislcurve = curves.value(curve) ;

    SISLCurve * newcurve = NULL ;
    int         stat ;

    s1712(sislcurve,
	  begin,
	  end,
	  &newcurve,
	  &stat) ;

    if (stat < 0) {
	cout << FG_RED << "Error occured inside call to SISL routine s1712: Picking a part of BSpline curve" << NOCOLOR << endl ;
    } else if (stat > 0) {
	cout << FG_MAGENTA << "WARNING: warning occured inside call to SISL routine s1712: Picking a part of BSpline curve." << NOCOLOR << endl;
    }

    return toQBSplineCurve(newcurve) ;
}

QPoint3f * QGeometricKernel::eval(QBSplineCurve * curve, float t, int derivative)
{
    SISLCurve * sislcurve = curves.value(curve) ;
    int         dimension = curve->idim ;
    int         leftknot;
    double      derive[(derivative+1)*dimension] ;
    int         stat ;

    s1227(sislcurve,
	  derivative,
	  t,
	  &leftknot,
	  derive,
	  &stat) ;

    if (stat < 0) {
	cout << FG_RED << "Error occured inside call to SISL routine s1127: Evaluating derivatives of a bspline curve" << NOCOLOR << endl ;
    } else if (stat > 0) {
	cout << FG_MAGENTA << "Warning occured inside call to SISL routine s1127: Evaluating derivatives of a bspline curve." << NOCOLOR << endl;
    }

    return new QPoint3f(derive[derivative*dimension+0], 
			derive[derivative*dimension+1], 
			derive[derivative*dimension+2]) ;
}

QPoint3f * QGeometricKernel::eval(QBSplineSurface * surface, float u, float v, int derivative)
{
    SISLSurf * sislsurface = surfaces.value(surface) ;
    int        dimension = surface->idim ;
    int        leftknot;
    int        rightknot;
    double     derive[(derivative+1)*(derivative+2)*dimension/2] ;
    double     normal[surface->idim] ;
    int        stat ;
    double     parvalue[] = {u, v} ;

    s1421(sislsurface,
	  derivative,
	  parvalue,
	  &leftknot,
	  &rightknot,
	  derive,
	  normal,
	  &stat) ;

    if (stat < 0) {
	cout << FG_RED << "Error occured inside call to SISL routine s1421: Evaluating derivatives of a bspline curve" << NOCOLOR << endl ;
    } else if (stat > 0) {
	cout << FG_MAGENTA << "Warning occured inside call to SISL routine s1421: Evaluating derivatives of a bspline curve." << NOCOLOR << endl;
    }

    return new QPoint3f(derive[derivative*dimension+0], 
			derive[derivative*dimension+1], 
			derive[derivative*dimension+2]) ;
}

QPoint3f * QGeometricKernel::normal(QBSplineSurface * surface, float u, float v)
{
    SISLSurf * sislsurface = surfaces.value(surface) ;
    int        dimension = surface->idim ;
    int derivative = 1 ;
    int        leftknot;
    int        rightknot;
    double     derive[(derivative+1)*(derivative+2)*dimension/2] ;
    double     normal[surface->idim] ;
    int        stat ;
    double     parvalue[] = {u, v} ;

    s1421(sislsurface,
	  derivative,
	  parvalue,
	  &leftknot,
	  &rightknot,
	  derive,
	  normal,
	  &stat) ;

    if (stat < 0) {
	cout << FG_RED << "Error occured inside call to SISL routine s1421: Evaluating derivatives of a bspline curve" << NOCOLOR << endl ;
    } else if (stat > 0) {
	cout << FG_MAGENTA << "Warning occured inside call to SISL routine s1421: Evaluating derivatives of a bspline curve." << NOCOLOR << endl;
    }
    return new QPoint3f(normal[0], 
			normal[1], 
			normal[2]) ;
}

void QGeometricKernel::normal(QBSplineSurface * surface, float u, float v, double * x, double * y, double * z)
{
    SISLSurf * sislsurface = surfaces.value(surface) ;
    int        dimension = surface->idim ;
    int derivative = 1 ;
    int        leftknot;
    int        rightknot;
    double     derive[(derivative+1)*(derivative+2)*dimension/2] ;
    double     normal[surface->idim] ;
    int        stat ;
    double     parvalue[] = {u, v} ;

    s1421(sislsurface,
	  derivative,
	  parvalue,
	  &leftknot,
	  &rightknot,
	  derive,
	  normal,
	  &stat) ;

    if (stat < 0) {
	cout << FG_RED << "Error occured inside call to SISL routine s1421: Evaluating derivatives of a bspline curve" << NOCOLOR << endl ;
    } else if (stat > 0) {
	cout << FG_MAGENTA << "Warning occured inside call to SISL routine s1421: Evaluating derivatives of a bspline curve." << NOCOLOR << endl;
    }

	*x = normal[0] ;
	*y = normal[1] ;
	*z = normal[2] ;
}

QBSplineCurve * QGeometricKernel::updt(QBSplineCurve * curve, QBSplineSurface * surface, float t, int dir)
{
    SISLSurf  * sislsurface = surfaces.value(surface) ;
    SISLCurve * sislcurve =   curves.value(curve) ;
    int         stat ;

    s1439(sislsurface, t, dir, &sislcurve, &stat) ;

    curve->setDirty(true) ;

    return curve ;
}

void QGeometricKernel::frenet(QBSplineCurve * curve, int sample)
{
    SISLCurve * sislcurve = curves.value(curve) ;

    int i = 0, jstat = 0 ;
    double ax[sample] ; for(float t = curve->umin() ; t <= curve->umax() ; t += (curve->umax()-curve->umin())/(sample-1), i++) ax[i] = t ;
    double p[3*sample], n[3*sample], t[3*sample], b[3*sample] ;
    s2559(sislcurve, ax, sample, p, t, n, b, &jstat) ;

    for(i = 0 ; i <= sample ; i++) {
	double pix = p[3*i+0], piy = p[3*i+1], piz = p[3*i+2] ;
	double tix = t[3*i+0], tiy = t[3*i+1], tiz = t[3*i+2] ;
	double nix = n[3*i+0], niy = n[3*i+1], niz = n[3*i+2] ;
	double bix = b[3*i+0], biy = b[3*i+1], biz = b[3*i+2] ;

	QPoint3f * p = new QPoint3f(pix, piy, piz) ;             p->setColor(Qt::lightGray) ;
	QArrow   * t = new QArrow  (p, new Vec(tix, tiy, tiz)) ; t->setColor(Qt::blue)      ;
	QArrow   * n = new QArrow  (p, new Vec(nix, niy, niz)) ; n->setColor(Qt::green)     ;
	QArrow   * b = new QArrow  (p, new Vec(bix, biy, biz)) ; b->setColor(Qt::red)       ;
	ObjectManager::instance()->addObject(p) ;
	ObjectManager::instance()->addObject(t) ;
	ObjectManager::instance()->addObject(n) ;
	ObjectManager::instance()->addObject(b) ;
    }
}
    
void QGeometricKernel:: frenet(QBSplineCurve * curve, double param, double* p, double* t, double* n, double* b)
{
    SISLCurve * sislcurve = curves.value(curve) ;
    int jstat = 0 ;
    s2559(sislcurve, &param, 1, p, t, n, b, &jstat) ;
}

void QGeometricKernel::frenetFrames(QBSplineCurve * curve, int sample, double * p, double * t, double * n, double * b) 
{
    SISLCurve * sislcurve = curves.value(curve) ;
    double      pertub = (curve->umax()-curve->umin())*1e-3;
    double      umin = curve->umin()+pertub ;
    double      umax = curve->umax()-pertub ;
    double      stepsize = (umax-umin)/(sample-1);
    int         jstat = 0 ;
    double      ax[sample] ; 

    for(int i=0; i<sample; ++i) ax[i] = i*stepsize;

    s2559(sislcurve, ax, sample, p, t, n, b, &jstat) ;
    QPoint3f* ps = eval(curve, umin);
    QPoint3f* pe = eval(curve, umax);

    p[0] = ps->x();
    p[1] = ps->y();
    p[2] = ps->z();

    p[(sample-1)*3+0] = pe->x();
    p[(sample-1)*3+1] = pe->y();
    p[(sample-1)*3+2] = pe->z();
}


void QGeometricKernel::rotationMinimizationFrames(QBSplineCurve * curve, int sample, double * p, double * t, double * n, double * b)
{
    frenetFrames(curve, sample, p, t, n, b);

    for(int i=0 ; i<sample-1 ; ++i) {
    	double tix = t[i*3+0];
    	double tiy = t[i*3+1];
    	double tiz = t[i*3+2];

    	double nix = n[i*3+0];
    	double niy = n[i*3+1];
    	double niz = n[i*3+2];

    	double v1x = p[3*(i+1)+0]-p[3*i+0];
    	double v1y = p[3*(i+1)+1]-p[3*i+1];
    	double v1z = p[3*(i+1)+2]-p[3*i+2];
    	double c1 = v1x*v1x + v1y*v1y + v1z*v1z;

    	double riLx = nix - (2.0/c1)*(v1x*nix+v1y*niy+v1z*niz)*v1x;
    	double riLy = niy - (2.0/c1)*(v1x*nix+v1y*niy+v1z*niz)*v1y;
    	double riLz = niz - (2.0/c1)*(v1x*nix+v1y*niy+v1z*niz)*v1z;

    	double tiLx = tix - (2.0/c1)*(v1x*tix+v1y*tiy+v1z*tiz)*v1x;
    	double tiLy = tiy - (2.0/c1)*(v1x*tix+v1y*tiy+v1z*tiz)*v1y;
    	double tiLz = tiz - (2.0/c1)*(v1x*tix+v1y*tiy+v1z*tiz)*v1z;

    	double v2x = t[3*(i+1)+0] - tiLx;
    	double v2y = t[3*(i+1)+1] - tiLy;
    	double v2z = t[3*(i+1)+2] - tiLz;
    	double c2 = v2x*v2x + v2y*v2y + v2z*v2z;

    	// compute new n and b
    	n[3*(i+1)+0] = riLx-(2.0/c2)*(v2x*riLx+v2y*riLy+v2z*riLz)*v2x;
    	n[3*(i+1)+1] = riLy-(2.0/c2)*(v2x*riLx+v2y*riLy+v2z*riLz)*v2y;
    	n[3*(i+1)+2] = riLz-(2.0/c2)*(v2x*riLx+v2y*riLy+v2z*riLz)*v2z;
      
    	b[3*(i+1)+0] = t[3*(i+1)+1]*n[3*(i+1)+2]-t[3*(i+1)+2]*n[3*(i+1)+1];
    	b[3*(i+1)+1] = t[3*(i+1)+2]*n[3*(i+1)+0]-t[3*(i+1)+0]*n[3*(i+1)+2];
    	b[3*(i+1)+2] = t[3*(i+1)+0]*n[3*(i+1)+1]-t[3*(i+1)+1]*n[3*(i+1)+0];
    }
}


void QGeometricKernel::rotationMinimizationFramesFree(QBSplineCurve * curve, QPoint3f * point, int sample, double * p, double * t, double * n, double * b)
{
  //frenetFrames(curve, sample, p, t, n, b);
   
  // double p[3*sample], n[3*sample], t[3*sample], b[3*sample] ;


  /*


  double      pertub = (curve->umax()-curve->umin())*1e-3;
  double      umin = curve->umin()+pertub ;
  double      umax = curve->umax()-pertub ;
  double      stepsize = (umax-umin)/sample;

   for(int i=0; i<sample; i++)
     { 

       float u=i*stepsize;
       Vector3f p_1=Vector3f(curve->eval(u)->x(), curve->eval(u)->y(),curve->eval(u)->z());
       Vector3f pder_1=Vector3f(curve->evalDer(u,1)->x(), curve->evalDer(u,1)->y(), curve->evalDer(u,1)->z());
       float a=pder_1.Length();
       Vector3f tt=pder_1*(1/a);
      
       p[3*i+0]=p_1.X();
       p[3*i+1]=p_1.Y(); 
       p[3*i+2]=p_1.Z();

       t[3*i+0]=tt.X();
       t[3*i+1]=tt.Y(); 
       t[3*i+2]=tt.Z();

     } 

   Vector3f t0=Vector3f(t[0],t[1],t[2]);
   Vector3f n1=Vector3f(point->x(), point->y(),point->z())-Vector3f(p[0],p[1],p[2]);
   Vector3f n_1=n1.Cross(t0);
   float c=n_1.Length();
   Vector3f nn=n_1*(1/c);

   Vector3f b_1=nn.Cross(t0);
   float d=b_1.Length();
   Vector3f bb=b_1*(1/d);
        
   n[0]=bb.X();
   n[1]=bb.Y(); 
   n[2]=bb.Z();
   
   b[0]=nn.X();
   b[1]=nn.Y(); 
   b[2]=nn.Z();	 
       
  
   for(int i=0 ; i<sample-1 ; ++i) {

        double tix = t[i*3+0];
    	double tiy = t[i*3+1];
    	double tiz = t[i*3+2];

    	double nix = n[i*3+0];
    	double niy = n[i*3+1];
    	double niz = n[i*3+2];

    	double v1x = p[3*(i+1)+0]-p[3*i+0];
    	double v1y = p[3*(i+1)+1]-p[3*i+1];
    	double v1z = p[3*(i+1)+2]-p[3*i+2];
    	double c1 = v1x*v1x + v1y*v1y + v1z*v1z;

    	double riLx = nix - (2.0/c1)*(v1x*nix+v1y*niy+v1z*niz)*v1x;
    	double riLy = niy - (2.0/c1)*(v1x*nix+v1y*niy+v1z*niz)*v1y;
    	double riLz = niz - (2.0/c1)*(v1x*nix+v1y*niy+v1z*niz)*v1z;

    	double tiLx = tix - (2.0/c1)*(v1x*tix+v1y*tiy+v1z*tiz)*v1x;
    	double tiLy = tiy - (2.0/c1)*(v1x*tix+v1y*tiy+v1z*tiz)*v1y;
    	double tiLz = tiz - (2.0/c1)*(v1x*tix+v1y*tiy+v1z*tiz)*v1z;

    	double v2x = t[3*(i+1)+0] - tiLx;
    	double v2y = t[3*(i+1)+1] - tiLy;
    	double v2z = t[3*(i+1)+2] - tiLz;
    	double c2 = v2x*v2x + v2y*v2y + v2z*v2z;

    	// compute new n and b
    	n[3*(i+1)+0] = riLx-(2.0/c2)*(v2x*riLx+v2y*riLy+v2z*riLz)*v2x;
    	n[3*(i+1)+1] = riLy-(2.0/c2)*(v2x*riLx+v2y*riLy+v2z*riLz)*v2y;
    	n[3*(i+1)+2] = riLz-(2.0/c2)*(v2x*riLx+v2y*riLy+v2z*riLz)*v2z;
      
    	b[3*(i+1)+0] = t[3*(i+1)+1]*n[3*(i+1)+2]-t[3*(i+1)+2]*n[3*(i+1)+1];
    	b[3*(i+1)+1] = t[3*(i+1)+2]*n[3*(i+1)+0]-t[3*(i+1)+0]*n[3*(i+1)+2];
    	b[3*(i+1)+2] = t[3*(i+1)+0]*n[3*(i+1)+1]-t[3*(i+1)+1]*n[3*(i+1)+0];
    }
  */
}


void QGeometricKernel::curvePairFrames(QBSplineCurve * curve1, QBSplineCurve * curve2, int sample)
{
  /* 
 double p[3*sample], l[3*sample], n[3*sample], t[3*sample], b[3*sample] ;
  
  float  step=1.0/sample;
  for(int i=0; i<=sample; i++)
     {
       float u=i*step;
       Vector3f p_1=Vector3f(curve1->eval(u)->x(), curve1->eval(u)->y(),curve1->eval(u)->z());
       Vector3f p_2=Vector3f(curve2->eval(u)->x(), curve2->eval(u)->y(),curve2->eval(u)->z());
       Vector3f pder_1=Vector3f(curve1->evalDer(u,1)->x(), curve1->evalDer(u,1)->y(), curve1->evalDer(u,1)->z());
       float a=pder_1.Length();
       Vector3f tt=pder_1*(1/a);
       
       Vector3f n1=p_2-p_1;
       Vector3f n_1=n1.Cross(pder_1);
       float c=n_1.Length();
       Vector3f nn=n_1*(1/c);

       Vector3f b_1=nn.Cross(tt);
       float d=b_1.Length();
       Vector3f bb=b_1*(1/d);
       
       p[3*i+0]=p_1.X();
       p[3*i+1]=p_1.Y(); 
       p[3*i+2]=p_1.Z();

       t[3*i+0]=tt.X();
       t[3*i+1]=tt.Y(); 
       t[3*i+2]=tt.Z();


       n[3*i+0]=nn.X();
       n[3*i+1]=nn.Y(); 
       n[3*i+2]=nn.Z();

       b[3*i+0]=bb.X();
       b[3*i+1]=bb.Y(); 
       b[3*i+2]=bb.Z();	 
	 
     }
   

  */
}


QBSplineSurface * QGeometricKernel::extrude(QBSplineCurve * curve, QBSplineCurve * path, int sample)
{
    SISLCurve * sislcurve = curves.value(curve) ;
    SISLCurve * sislpath  = curves.value(path)  ;

    SISLCurve * dbase = copyCurve(sislcurve) ;
    SISLCurve * dpath = copyCurve(sislpath) ;

    for(int j = 0 ; j < dpath->in ; j++) {
	dpath->ecoef[3*j+0] -= sislpath->ecoef[0] ;
	dpath->ecoef[3*j+1] -= sislpath->ecoef[1] ;
	dpath->ecoef[3*j+2] -= sislpath->ecoef[2] ;
    }

    for(int j = 0 ; j < dbase->in ; j++) {
	dbase->ecoef[3*j+0] -= sislpath->ecoef[0] ;
	dbase->ecoef[3*j+1] -= sislpath->ecoef[1] ;
	dbase->ecoef[3*j+2] -= sislpath->ecoef[2] ;
    }

    SISLCurve * vpcurv[sample] ; 
    int         nctype[sample] ;
    int         jstat = 0 ;

    double p[3*sample], n[3*sample], t[3*sample], b[3*sample] ;
    rotationMinimizationFrames(path, sample, p, t, n, b) ;

    double p0x = p[0], p0y = p[1], p0z = p[2] ;
    double t0x = t[0], t0y = t[1], t0z = t[2] ;
    double n0x = n[0], n0y = n[1], n0z = n[2] ;
    double b0x = b[0], b0y = b[1], b0z = b[2] ;

    for(int i = 0 ; i < sample ; i++) {
    	SISLCurve * dcurve = copyCurve(dbase) ;
    	double pix = p[3*i+0], piy = p[3*i+1], piz = p[3*i+2] ;
    	double tix = t[3*i+0], tiy = t[3*i+1], tiz = t[3*i+2] ;
    	double nix = n[3*i+0], niy = n[3*i+1], niz = n[3*i+2] ;
    	double bix = b[3*i+0], biy = b[3*i+1], biz = b[3*i+2] ;
	
    	for(int j = 0 ; j < dcurve->in ; j++) {
    	    double x = dcurve->ecoef[3*j+0] ;
    	    double y = dcurve->ecoef[3*j+1] ;
    	    double z = dcurve->ecoef[3*j+2] ;
    	    dcurve->ecoef[3*j+0] = x*tix*t0x+x*nix*n0x+x*bix*b0x+y*tix*t0y+y*nix*n0y+y*bix*b0y+z*tix*t0z+z*nix*n0z+z*bix*b0z-tix*p0x-nix*p0y-bix*p0z+pix+sislpath->ecoef[0] ;
    	    dcurve->ecoef[3*j+1] = x*tiy*t0x+x*niy*n0x+x*biy*b0x+y*tiy*t0y+y*niy*n0y+y*biy*b0y+z*tiy*t0z+z*niy*n0z+z*biy*b0z-tiy*p0x-niy*p0y-biy*p0z+piy+sislpath->ecoef[1] ;
    	    dcurve->ecoef[3*j+2] = x*tiz*t0x+x*niz*n0x+x*biz*b0x+y*tiz*t0y+y*niz*n0y+y*biz*b0y+z*tiz*t0z+z*niz*n0z+z*biz*b0z-tiz*p0x-niz*p0y-biz*p0z+piz+sislpath->ecoef[2] ;
    	}
    	vpcurv[i] = dcurve ;
    	nctype[i] = 1 ;

    	 QPoint3f * p = new QPoint3f(pix+sislpath->ecoef[0], piy+sislpath->ecoef[1], piz+sislpath->ecoef[2]) ;
    	 QArrow   * t = new QArrow  (p, new Vec(tix, tiy, tiz)) ; t->setColor(Qt::blue)  ;
    	 QArrow   * n = new QArrow  (p, new Vec(nix, niy, niz)) ; n->setColor(Qt::green) ;
    	 QArrow   * b = new QArrow  (p, new Vec(bix, biy, biz)) ; b->setColor(Qt::red)   ;
    	 ObjectManager::instance()->addObject(p) ;
    	 ObjectManager::instance()->addObject(t) ;
    	 ObjectManager::instance()->addObject(n) ;
    	 ObjectManager::instance()->addObject(b) ;
    	 ObjectManager::instance()->addObject(toQBSplineCurve(dcurve)) ;
    }

    SISLSurf * rsurf = NULL ; 
    double   *  gpar = NULL ; 
    s1538(sample, vpcurv, nctype, path->umin(), 1, curve->order, 1, &rsurf, &gpar, &jstat) ; //lofting B-spline surface

    for(int i = 0 ; i < sample ; i++) freeCurve(vpcurv[i]) ;
    freeCurve(dbase) ;
    freeCurve(dpath) ;

    return toQBSplineSurface(rsurf) ;
}


QBSplineSurface * QGeometricKernel::extrudeFree(QBSplineCurve * curve, QPoint3f * point, QBSplineCurve * path, int sample)
{
  /*

    SISLCurve * sislcurve = curves.value(curve) ;
    SISLCurve * sislpath  = curves.value(path)  ;

    SISLCurve * dbase = copyCurve(sislcurve) ;
    SISLCurve * dpath = copyCurve(sislpath) ;

    for(int j = 0 ; j < dpath->in ; j++) {
	dpath->ecoef[3*j+0] -= sislpath->ecoef[0] ;
	dpath->ecoef[3*j+1] -= sislpath->ecoef[1] ;
	dpath->ecoef[3*j+2] -= sislpath->ecoef[2] ;
    }

    for(int j = 0 ; j < dbase->in ; j++) {
	dbase->ecoef[3*j+0] -= sislpath->ecoef[0] ;
	dbase->ecoef[3*j+1] -= sislpath->ecoef[1] ;
	dbase->ecoef[3*j+2] -= sislpath->ecoef[2] ;
    }

    SISLCurve * vpcurv[sample] ; 
    int         nctype[sample] ;
    int         jstat = 0 ;

    double p[3*sample], n[3*sample], t[3*sample], b[3*sample] ; int i = 0 ;
    rotationMinimizationFramesFree(path, point, sample, p, t, n, b) ;

    double p0x = p[0], p0y = p[1], p0z = p[2] ;
    double t0x = t[0], t0y = t[1], t0z = t[2] ;
    double n0x = n[0], n0y = n[1], n0z = n[2] ;
    double b0x = b[0], b0y = b[1], b0z = b[2] ;

    for(int i = 0 ; i < sample ; i++) {
    	SISLCurve * dcurve = copyCurve(dbase) ;
    	double pix = p[3*i+0], piy = p[3*i+1], piz = p[3*i+2] ;
    	double tix = t[3*i+0], tiy = t[3*i+1], tiz = t[3*i+2] ;
    	double nix = n[3*i+0], niy = n[3*i+1], niz = n[3*i+2] ;
    	double bix = b[3*i+0], biy = b[3*i+1], biz = b[3*i+2] ;
	
    	for(int j = 0 ; j < dcurve->in ; j++) {
    	    double x = dcurve->ecoef[3*j+0] ;
    	    double y = dcurve->ecoef[3*j+1] ;
    	    double z = dcurve->ecoef[3*j+2] ;
    	    dcurve->ecoef[3*j+0] = x*tix*t0x+x*nix*n0x+x*bix*b0x+y*tix*t0y+y*nix*n0y+y*bix*b0y+z*tix*t0z+z*nix*n0z+z*bix*b0z-tix*p0x-nix*p0y-bix*p0z+pix+sislpath->ecoef[0] ;
    	    dcurve->ecoef[3*j+1] = x*tiy*t0x+x*niy*n0x+x*biy*b0x+y*tiy*t0y+y*niy*n0y+y*biy*b0y+z*tiy*t0z+z*niy*n0z+z*biy*b0z-tiy*p0x-niy*p0y-biy*p0z+piy+sislpath->ecoef[1] ;
    	    dcurve->ecoef[3*j+2] = x*tiz*t0x+x*niz*n0x+x*biz*b0x+y*tiz*t0y+y*niz*n0y+y*biz*b0y+z*tiz*t0z+z*niz*n0z+z*biz*b0z-tiz*p0x-niz*p0y-biz*p0z+piz+sislpath->ecoef[2] ;
    	}
    	vpcurv[i] = dcurve ;
    	nctype[i] = 1 ;

    	 QPoint3f * p = new QPoint3f(pix+sislpath->ecoef[0], piy+sislpath->ecoef[1], piz+sislpath->ecoef[2]) ;
    	 QArrow   * t = new QArrow  (p, new Vec(tix, tiy, tiz)) ; t->setColor(Qt::blue)  ;
    	 QArrow   * n = new QArrow  (p, new Vec(nix, niy, niz)) ; n->setColor(Qt::green) ;
    	 QArrow   * b = new QArrow  (p, new Vec(bix, biy, biz)) ; b->setColor(Qt::red)   ;
    	 ObjectManager::instance()->addObject(p) ;
    	 ObjectManager::instance()->addObject(t) ;
    	 ObjectManager::instance()->addObject(n) ;
    	 ObjectManager::instance()->addObject(b) ;
    	 ObjectManager::instance()->addObject(toQBSplineCurve(dcurve)) ;
    }

    SISLSurf * rsurf = NULL ; 
    double   *  gpar = NULL ; 
    s1538(sample, vpcurv, nctype, path->umin(), 1, curve->order, 1, &rsurf, &gpar, &jstat) ; //lofting B-spline surface

    for(int i = 0 ; i < sample ; i++) freeCurve(vpcurv[i]) ;
    freeCurve(dbase) ;
    freeCurve(dpath) ;

    return toQBSplineSurface(rsurf) ;
  */
}



QBSplineSurface * QGeometricKernel::extrudeCPF(QBSplineCurve * curve, QBSplineCurve * path1, QBSplineCurve * path2, int sample)
{
  /*
    SISLCurve * sislcurve = curves.value(curve) ;
    SISLCurve * sislpath  = curves.value(path1)  ;

    SISLCurve * dbase = copyCurve(sislcurve) ;
    SISLCurve * dpath = copyCurve(sislpath) ;

    for(int j = 0 ; j < dpath->in ; j++) {
	dpath->ecoef[3*j+0] -= sislpath->ecoef[0] ;
	dpath->ecoef[3*j+1] -= sislpath->ecoef[1] ;
	dpath->ecoef[3*j+2] -= sislpath->ecoef[2] ;
    }

    for(int j = 0 ; j < dbase->in ; j++) {
	dbase->ecoef[3*j+0] -= sislpath->ecoef[0] ;
	dbase->ecoef[3*j+1] -= sislpath->ecoef[1] ;
	dbase->ecoef[3*j+2] -= sislpath->ecoef[2] ;
    }

    SISLCurve * vpcurv[sample] ; 
    int         nctype[sample] ;
    int         jstat = 0 ;

    // double p[3*sample], n[3*sample], t[3*sample], b[3*sample] ; int i = 0 ;
    // rotationMinimizationFrames(path, sample, p, t, n, b) ;

    //compute the Curve-pair frames 

  double p[3*sample], l[3*sample], n[3*sample], t[3*sample], b[3*sample] ;
  int i=0;
  
  float  step=1.0/sample;
  for(int i=0; i<=sample; i++)
     {
       float u=i*step;
       Vector3f p_1=Vector3f(path1->eval(u)->x(), path1->eval(u)->y(),path1->eval(u)->z());
       Vector3f p_2=Vector3f(path2->eval(u)->x(), path2->eval(u)->y(),path2->eval(u)->z());
       Vector3f pder_1=Vector3f(path1->evalDer(u,1)->x(), path1->evalDer(u,1)->y(), path1->evalDer(u,1)->z());
       float a=pder_1.Length();
       Vector3f tt=pder_1*(1/a);
       
       Vector3f n1=p_1-p_2;
       Vector3f n_1=n1.Cross(pder_1);
       float c=n_1.Length();
       Vector3f nn=n_1*(1/c);

       Vector3f b_1=nn.Cross(tt);
       float d=b_1.Length();
       Vector3f bb=b_1*(1/d);
       
       p[3*i+0]=p_1.X();
       p[3*i+1]=p_1.Y(); 
       p[3*i+2]=p_1.Z();

       t[3*i+0]=tt.X();
       t[3*i+1]=tt.Y(); 
       t[3*i+2]=tt.Z();


       n[3*i+0]=bb.X();
       n[3*i+1]=bb.Y(); 
       n[3*i+2]=bb.Z();

       b[3*i+0]=nn.X();
       b[3*i+1]=nn.Y(); 
       b[3*i+2]=nn.Z();	 
	 
     }
   

    double p0x = p[0], p0y = p[1], p0z = p[2] ;
    double t0x = t[0], t0y = t[1], t0z = t[2] ;
    double n0x = n[0], n0y = n[1], n0z = n[2] ;
    double b0x = b[0], b0y = b[1], b0z = b[2] ;
    
    //vpcurv[0]=sislcurve;
    // nctype[0]=1;

    for(int i= 10 ; i <sample ; i++) {
    	SISLCurve * dcurve = copyCurve(dbase) ;
    	double pix = p[3*i+0], piy = p[3*i+1], piz = p[3*i+2] ;
    	double tix = t[3*i+0], tiy = t[3*i+1], tiz = t[3*i+2] ;
    	double nix = n[3*i+0], niy = n[3*i+1], niz = n[3*i+2] ;
    	double bix = b[3*i+0], biy = b[3*i+1], biz = b[3*i+2] ;
	
    	for(int j = 0 ; j < dcurve->in ; j++) {
    	    double x = dcurve->ecoef[3*j+0] ;
    	    double y = dcurve->ecoef[3*j+1] ;
    	    double z = dcurve->ecoef[3*j+2] ;
	    dcurve->ecoef[3*j+0] = x*tix*t0x+x*nix*n0x+x*bix*b0x+y*tix*t0y+y*nix*n0y+y*bix*b0y+z*tix*t0z+z*nix*n0z+z*bix*b0z-tix*p0x-nix*p0y-bix*p0z+pix+sislpath->ecoef[0] ;
    	    dcurve->ecoef[3*j+1] = x*tiy*t0x+x*niy*n0x+x*biy*b0x+y*tiy*t0y+y*niy*n0y+y*biy*b0y+z*tiy*t0z+z*niy*n0z+z*biy*b0z-tiy*p0x-niy*p0y-biy*p0z+piy+sislpath->ecoef[1] ;
    	    dcurve->ecoef[3*j+2] = x*tiz*t0x+x*niz*n0x+x*biz*b0x+y*tiz*t0y+y*niz*n0y+y*biz*b0y+z*tiz*t0z+z*niz*n0z+z*biz*b0z-tiz*p0x-niz*p0y-biz*p0z+piz+sislpath->ecoef[2] ;
    	}
    	vpcurv[i-10] = dcurve ;
    	nctype[i-10] = 1 ;

    	 QPoint3f * p = new QPoint3f(pix, piy, piz) ;
    	 QArrow   * t = new QArrow  (p, new Vec(tix, tiy, tiz)) ; t->setColor(Qt::blue)  ;
    	 QArrow   * n = new QArrow  (p, new Vec(nix, niy, niz)) ; n->setColor(Qt::green) ;
    	 QArrow   * b = new QArrow  (p, new Vec(bix, biy, biz)) ; b->setColor(Qt::red)   ;
    	 ObjectManager::instance()->addObject(p) ;
    	 ObjectManager::instance()->addObject(t) ;
    	 ObjectManager::instance()->addObject(n) ;
    	 ObjectManager::instance()->addObject(b) ;
    	 ObjectManager::instance()->addObject(toQBSplineCurve(dcurve)) ;
    }

    SISLSurf * rsurf = NULL ; 
    double   *  gpar = NULL ; 
    s1538(sample-10, vpcurv, nctype, path1->umin(), 1, curve->order, 1, &rsurf, &gpar, &jstat) ; //lofting B-spline surface

    for(int i = 10 ; i < sample ; i++) freeCurve(vpcurv[i-10]) ;
    freeCurve(dbase) ;
    freeCurve(dpath) ;

    return toQBSplineSurface(rsurf) ;  
  */  
}


double QGeometricKernel::closestParameter(QBSplineCurve * curve, QPoint3f * p)
{
  SISLCurve * sislcurve = curves.value(curve) ;
  double epoint[3] = { p->x(), p->y(), p->z() } ;
  double epsco = 1.0e-6 ;
  double epsge = 1.0e-6 ;
  
  double gpar ;     // parameter value
  double dist ;     // distance value
  int jstat ;       // status message

  s1957(sislcurve, epoint, 3, epsco, epsge, &gpar, &dist, &jstat) ;

  if(jstat < 0) 
    cout << "error in closestParameter" << endl ;

  return gpar ;
}

QBSplineSurface * QGeometricKernel::loft(QBSplineCurve * curve, int sample, QList<QPoint3f*>& points, QList<double>& radius) 
{
    int         np = points.size() ;
    double      param[np] ;
    int         nctype[sample] ;
    SISLCurve * vpcurve[sample] ;
    SISLCurve * basesislcurve ; 
    double      p[3*sample], n[3*sample], t[3*sample], b[3*sample] ;

    // create a template base curve, with radius = 1;
    QList<QPoint3f*> base ;
    int num = 10; // number of base points
    for(int k=0; k<num; ++k) {
      double x = cos(k*2*M_PI/num);
      double y = sin(k*2*M_PI/num);
      double z = 0;
      base << new QPoint3f(x, y, z);
    }
    base << base[0] ; 
    QBSplineCurve * basecurve = interpolate(base) ;
    foreach(QPoint3f * pt, basecurve->points)
      pt->setSize(0.005) ;
    basesislcurve = curves.value(basecurve);

    // compute parameter for each data point    
    for(int i=0; i<np; ++i)
        param[i] = closestParameter(curve, points[i]) ;

    rotationMinimizationFrames(curve, sample, p, t, n, b) ;

    double      pertub = (curve->umax()-curve->umin())*1e-3;
    double      umin = curve->umin()+pertub ;
    double      umax = curve->umax()-pertub ;
    double      stepsize = (umax-umin)/(sample-1);

    for(int i = 0 ; i < sample ; ++i) {
	double ax = i*stepsize;
	double r = 1e-4;

	// find interval of current sample to interpolate radius
	if(ax <= param[0])
	    r = radius[0];
	else if (ax >= param[np-1])
	    r = radius[np-1];
	else {
	    for(int j=0; j<np-1; ++j)
		if(ax >= param[j] && ax<param[j+1]) {
		    double ratio = (ax-param[j])/(param[j+1]-param[j]);
		    r = radius[j]*(1-ratio) + radius[j+1]*ratio;
		    break;
		}
	}

    	SISLCurve * dcurve = copyCurve(basesislcurve) ;
    	double pix = p[3*i+0], piy = p[3*i+1], piz = p[3*i+2] ;
    	double tix = t[3*i+0], tiy = t[3*i+1], tiz = t[3*i+2] ;
    	double nix = n[3*i+0], niy = n[3*i+1], niz = n[3*i+2] ;
    	double bix = b[3*i+0], biy = b[3*i+1], biz = b[3*i+2] ;
	
    	for(int j = 0 ; j < dcurve->in ; j++) {
    	    double x = dcurve->ecoef[3*j+0] * r ;
    	    double y = dcurve->ecoef[3*j+1] * r ;
    	    double z = dcurve->ecoef[3*j+2] * r ;
	    
    	    dcurve->ecoef[3*j+0] = x*nix + y*bix + z*tix + pix; 
    	    dcurve->ecoef[3*j+1] = x*niy + y*biy + z*tiy + piy;
    	    dcurve->ecoef[3*j+2] = x*niz + y*biz + z*tiz + piz; 
    	}
    	vpcurve[i] = dcurve ;
    	nctype[i] = 1 ;

	//    	QPoint3f * p = new QPoint3f(pix, piy, piz) ; p->setSize(0.003) ;
//    	QArrow   * t = new QArrow  (p, new Vec(tix*0.03, tiy*0.03, tiz*0.03), 0.001) ; t->setColor(Qt::blue)  ;
//    	QArrow   * n = new QArrow  (p, new Vec(nix*0.03, niy*0.03, niz*0.03), 0.001) ; n->setColor(Qt::green) ;
//    	QArrow   * b = new QArrow  (p, new Vec(bix*0.03, biy*0.03, biz*0.03), 0.001) ; b->setColor(Qt::red)   ;
//    	ObjectManager::instance()->addObject(p) ;
//    	ObjectManager::instance()->addObject(t) ;
//    	ObjectManager::instance()->addObject(n) ;
//    	ObjectManager::instance()->addObject(b) ;
//   	ObjectManager::instance()->addObject(toQBSplineCurve(dcurve)) ;
    }

    SISLSurf * rsurf = NULL ; 
    double   * gpar = NULL ; 
    int        jstat = 0 ;
    s1538(sample, vpcurve, nctype, curve->umin(), 1, curve->order, 1, &rsurf, &gpar, &jstat) ;

    for(int i = 0 ; i < sample ; i++) freeCurve(vpcurve[i]) ;

    return toQBSplineSurface(rsurf) ;
}

QBSplineSurface * QGeometricKernel::loftFrenet(QBSplineCurve * curve, int sample, QList<QPoint3f*>& points, QList<double>& radius) 
{
    int         np = points.size() ;
    double      param[np] ;
    int         nctype[sample] ;
    SISLCurve * vpcurve[sample] ;
    SISLCurve * basesislcurve ; 
    double      p[3*sample], n[3*sample], t[3*sample], b[3*sample] ;

    // create a template base curve, with radius = 1;
    QList<QPoint3f*> base ;
    int num = 10; // number of base points
    for(int k=0; k<num; ++k) {
      double x = cos(k*2*M_PI/num);
      double y = sin(k*2*M_PI/num);
      double z = 0;
      base << new QPoint3f(x, y, z);
    }
    base << base[0] ; 
    QBSplineCurve * basecurve = interpolate(base) ;
    foreach(QPoint3f * pt, basecurve->points)
      pt->setSize(0.005) ;
    basesislcurve = curves.value(basecurve);

    // compute parameter for each data point    
    for(int i=0; i<np; ++i)
        param[i] = closestParameter(curve, points[i]) ;

    frenetFrames(curve, sample, p, t, n, b) ;

    double      pertub = (curve->umax()-curve->umin())*1e-3;
    double      umin = curve->umin()+pertub ;
    double      umax = curve->umax()-pertub ;
    double      stepsize = (umax-umin)/(sample-1);

    for(int i = 0 ; i < sample ; ++i) {
	double ax = i*stepsize;
	double r = 1e-4;

	// find interval of current sample to interpolate radius
	if(ax <= param[0])
	    r = radius[0];
	else if (ax >= param[np-1])
	    r = radius[np-1];
	else {
	    for(int j=0; j<np-1; ++j)
		if(ax >= param[j] && ax<param[j+1]) {
		    double ratio = (ax-param[j])/(param[j+1]-param[j]);
		    r = radius[j]*(1-ratio) + radius[j+1]*ratio;
		    break;
		}
	}

    	SISLCurve * dcurve = copyCurve(basesislcurve) ;
    	double pix = p[3*i+0], piy = p[3*i+1], piz = p[3*i+2] ;
    	double tix = t[3*i+0], tiy = t[3*i+1], tiz = t[3*i+2] ;
    	double nix = n[3*i+0], niy = n[3*i+1], niz = n[3*i+2] ;
    	double bix = b[3*i+0], biy = b[3*i+1], biz = b[3*i+2] ;
	
    	for(int j = 0 ; j < dcurve->in ; j++) {
    	    double x = dcurve->ecoef[3*j+0] * r ;
    	    double y = dcurve->ecoef[3*j+1] * r ;
    	    double z = dcurve->ecoef[3*j+2] * r ;

    	    dcurve->ecoef[3*j+0] = x*nix + y*bix + z*tix + pix; 
    	    dcurve->ecoef[3*j+1] = x*niy + y*biy + z*tiy + piy;
    	    dcurve->ecoef[3*j+2] = x*niz + y*biz + z*tiz + piz; 
    	}
    	vpcurve[i] = dcurve ;
    	nctype[i] = 1 ;

	//    	QPoint3f * p = new QPoint3f(pix, piy, piz) ; p->setSize(0.003) ;
//    	QArrow   * t = new QArrow  (p, new Vec(tix*0.03, tiy*0.03, tiz*0.03), 0.001) ; t->setColor(Qt::blue)  ;
//    	QArrow   * n = new QArrow  (p, new Vec(nix*0.03, niy*0.03, niz*0.03), 0.001) ; n->setColor(Qt::green) ;
//    	QArrow   * b = new QArrow  (p, new Vec(bix*0.03, biy*0.03, biz*0.03), 0.001) ; b->setColor(Qt::red)   ;
//    	ObjectManager::instance()->addObject(p) ;
//    	ObjectManager::instance()->addObject(t) ;
//    	ObjectManager::instance()->addObject(n) ;
//    	ObjectManager::instance()->addObject(b) ;
//   	ObjectManager::instance()->addObject(toQBSplineCurve(dcurve)) ;
    }

    SISLSurf * rsurf = NULL ; 
    double   * gpar = NULL ; 
    int        jstat = 0 ;
    s1538(sample, vpcurve, nctype, curve->umin(), 1, curve->order, 1, &rsurf, &gpar, &jstat) ;

    for(int i = 0 ; i < sample ; i++) freeCurve(vpcurve[i]) ;

    return toQBSplineSurface(rsurf) ;
}

void QGeometricKernel::medial(QBSplineCurve * c1, QBSplineCurve * c2)
{
    SISLCurve * sislcurve1 = curves.value(c1) ;
    SISLCurve * sislcurve2 = curves.value(c2) ;

    double epsco = 1e-6 ; // Computational resolution
    double epsge = 1e-6 ; // Geometric     resolution

    int numintpt ;              // Number of single closest points
    double * intpar1 ;          // Array containing the parameter
				// values of the single closest points
				// in the parameter interval of the
				// first curve. The points lie in
				// sequence. Closest curves are stored
				// in intcurve.
    double * intpar2 ;          // Array containing the parameter
				// values of the single closest points
				// in the parameter interval of the
				// second curve. The points lie in
				// sequence. Closest curves are stored
				// in intcurve.
    int numintcu ;              // Number of closest curves.
    SISLIntcurve ** intcurve ;  // Array of pointers to the
				// SISLIntcurve ob jects containing
				// descriptions of the closest
				// curves. The curves are only
				// described by start points and end
				// points in the parameter interval of
				// the curve. The curve pointers point
				// to nothing. If the curves given as
				// input are degenerate, a closest
				// point may be returned as a closest
				// curve.
    int stat ; 

    s1955(sislcurve1, sislcurve2, epsco, epsge, &numintpt, &intpar1, &intpar2, 
	  &numintcu, &intcurve, &stat) ;

    for(int i = 0 ; i < numintpt ; i++) {
	cout << "pt" << i << ": par1 = " << intpar1[i] << endl ;
	cout << "pt" << i << ": par2 = " << intpar2[i] << endl ;
	QPoint3f * p1 = eval(c1, intpar1[i], 0) ; p1->setColor(Qt::cyan) ;
	QPoint3f * p2 = eval(c2, intpar2[i], 0) ; p2->setColor(Qt::magenta) ;
	ObjectManager::instance()->addObject(p1) ;
	ObjectManager::instance()->addObject(p2) ;
    }

    for(int i = 0 ; i < numintcu ; i++) {
	printSislIntCurve(intcurve[i]) ;
    }
}

QPlane * QGeometricKernel::tangentPlane(QBSplineSurface * surface, float u, float v)
{
    QPoint3f * n = this->normal(surface, u, v) ;
    QPoint3f * p = this->eval(surface, u, v, 0) ;
    float a = n->x() ;
    float b = n->y() ;
    float c = n->z() ;
    float d = -a*p->x()-b*p->y()-c*p->z() ;
    return new QPlane(a, b, c, d, 1) ;
}


 
void QGeometricKernel::draw_z(QSurfaceColorMap * s)
{
  SplineSurface * ss=QBSplineSurfacetoGo(s->surf);
  
      
  int m = 50;
  int n = 50;
  IGLGood * result = new IGLGood(IGLGood::E_QUAD,m*n,4*(m-1)*(n-1),true) ;

  hsv_colormap.set_value_range(s->zmax, s->zmin);
  
  
  float mc[3];
  
  for(int i = 0 ; i < m ; i++) {
    float u = ss->startparam_u() + i*(ss->endparam_u()-ss->startparam_u())/(m-1) ;
    for(int j = 0 ; j < n ; j++) {
      float v = ss->startparam_v() + j*(ss->endparam_v()-ss->startparam_v())/(n-1) ;
      
      Go::Point pt1;
      ss->point(pt1,u,v);
     // cout<<pt1<<"   "<<"    "<<u<<"    "<<v<<endl;
     // cout<<pt1[0]<<"    "<<pt1[1]<<"    "<<pt1[2]<<"    "<<pt1[3]<<endl;
      getcolor(pt1[2],mc);      

      result->colors[3*m*i+3*j]=mc[0];
      result->colors[3*m*i+3*j+1]=mc[1];
      result->colors[3*m*i+3*j+2]=mc[2];
      result->vertices[3*m*i+3*j]=pt1[0];
      result->vertices[3*m*i+3*j+1]=pt1[1];
      result->vertices[3*m*i+3*j+2]=pt1[2];
     
      Go::Point pt2;
      ss->normal(pt2,u,v);
      result->normals[3*m*i+3*j]=pt2[0];
      result->normals[3*m*i+3*j+1]=pt2[1];
      result->normals[3*m*i+3*j+2]=pt2[2];
    }
  }
  
  int c = 0 ;
  for(int i = 0 ; i < m-1; i ++)
    for(int j = 0 ; j < n-1; j ++, c += 4) {
      int a = i*n+j ;
      result->indexes[c] = a ;
      result->indexes[c+1] = a + n ;
      result->indexes[c+2] = a + n + 1 ;
      result->indexes[c+3] = a + 1 ;
    }
  // cout << "filled indexes" << endl ;
   
  glEnableClientState(GL_VERTEX_ARRAY) ;
  glVertexPointer(3, GL_DOUBLE, 0, result->vertices) ;
  
  glEnableClientState(GL_COLOR_ARRAY) ;   
  glColorPointer(3,GL_FLOAT, 0, result->colors) ; 
  
  glEnableClientState(GL_NORMAL_ARRAY) ;   
  glNormalPointer(GL_DOUBLE, 0, result->normals) ;
  
  glDisable(GL_LIGHTING) ;
  glDrawElements(GL_QUADS, result->nbi, GL_UNSIGNED_INT, result->indexes) ;
  glEnable(GL_LIGHTING) ;
  glEnable(GL_COLOR_MATERIAL);

  glDisableClientState(GL_VERTEX_ARRAY) ;
  glDisableClientState(GL_COLOR_ARRAY) ;
  glDisableClientState(GL_NORMAL_ARRAY) ;


/*for(int i1=0;i1<=100;i1++)
    {
        Go::Point pt3;
      ss->point(pt3,i1,1);
      cout<<pt3<<"  1 "<<"  1  "<<1<<"   1 "<<1<<endl;
  }*/

}


//function for getting RGB information from z-value c 

void  QGeometricKernel::getcolor(double c,  float* mc)
{
 
 double R,G,B;

 hsv_colormap.value2rgb(c,R,G,B);
 mc[0] = (float)R;
 mc[1] = (float)G;
 mc[2] = (float)B;

}

//functions for QExactSolutionSurface
 
void QGeometricKernel::draw_exact(QExactSolutionSurface * s)
{
 
  SplineSurface * ss=QBSplineSurfacetoGo(s->surf);
  
  int m = 50;
  int n = 50;
  IGLGood * result = new IGLGood(IGLGood::E_QUAD,m*n,4*(m-1)*(n-1),true) ;
  
  //filled vertices 
  hsv_colormap.set_value_range(s->zmax, s->zmin);
  
  float mc[3];
  
  for(int i = 0 ; i < m ; i++) {
    float u = ss->startparam_u() + i*(ss->endparam_u()-ss->startparam_u())/(m-1) ;
    for(int j = 0 ; j < n ; j++) {
      float v = ss->startparam_v() + j*(ss->endparam_v()-ss->startparam_v())/(n-1) ;

      Go::Point pt1;
      ss->point(pt1,u,v);

      result->vertices[3*m*i+3*j]=pt1[0];
      result->vertices[3*m*i+3*j+1]=pt1[1];
      //  float a=3.14156;
      //  float x=pt1[0];
      //  float y=pt1[1];
      //  float exact=sin(a*(y-x*x))*sin(a*x)*sin(a*y);
      float src=-4*3.14156*3.14156*sin(3.14156*pt1[0]/3) * sin(3.14156*pt1[1]/3)/9;     
      float exact=-36*src/(8*3.14156*3.14156);
      result->vertices[3*m*i+3*j+2]=exact;
      getcolor(exact,mc);
      result->colors[3*m*i+3*j]=mc[0];
      result->colors[3*m*i+3*j+1]=mc[1];
      result->colors[3*m*i+3*j+2]=mc[2];
    
      Go::Point pt2;
      ss->normal(pt2,u,v);
      result->normals[3*m*i+3*j]=pt2[0];
      result->normals[3*m*i+3*j+1]=pt2[1];
      result->normals[3*m*i+3*j+2]=pt2[2];
    }
  }
  
  int c = 0 ;
  for(int i = 0 ; i < m-1; i ++)
    for(int j = 0 ; j < n-1; j ++, c += 4) {
      int a = i*n+j ;
      result->indexes[c] = a ;
      result->indexes[c+1] = a + n ;
      result->indexes[c+2] = a + n + 1 ;
      result->indexes[c+3] = a + 1 ;
    }
  // cout << "filled indexes" << endl ;
   
 
  glEnableClientState(GL_VERTEX_ARRAY) ;
  glVertexPointer(3, GL_DOUBLE, 0, result->vertices) ;
  
  glEnableClientState(GL_COLOR_ARRAY) ;   
  glColorPointer(3,GL_FLOAT, 0, result->colors) ; 
  
  glEnableClientState(GL_NORMAL_ARRAY) ;   
  glNormalPointer(GL_DOUBLE, 0, result->normals) ;
  
  glDisable(GL_LIGHTING) ;
  glDrawElements(GL_QUADS, result->nbi, GL_UNSIGNED_INT, result->indexes) ;
  glEnable(GL_LIGHTING) ;
  glEnable(GL_COLOR_MATERIAL);

  glDisableClientState(GL_VERTEX_ARRAY) ;
  glDisableClientState(GL_COLOR_ARRAY) ;
  glDisableClientState(GL_NORMAL_ARRAY) ;
  
}

QPoint4f * QGeometricKernel::eval(QAxel::QExactSolutionVolume * bv, float u, float v, float w)
{
  SplineVolume * vv=QBSplineVolumetoGo(bv->vol);
  Go::Point pt;
  vv->point(pt,u,v,w);
  
  //  float a=3.14156;
  //   float x=pt[0];
  //   float y=pt[1];
  //   float exact=sin(a*(y-x*x))*sin(a*x)*sin(a*y);
  float exact=sin(3.14156*pt[0]/3) * sin(3.14156*pt[1]/3)*sin(3.14156*pt[2]/3);
   
  return new QPoint4f(pt[0],pt[1],pt[2],exact);   
}

QPoint3f * QGeometricKernel::eval(QAxel::QExactSolutionSurface * s, float u, float v)
{
  SplineSurface * ss=QBSplineSurfacetoGo(s->surf);
  Go::Point pt;
  ss->point(pt,u,v);
  
  //  float a=3.14156;
  //   float x=pt[0];
  //   float y=pt[1];
  //   float exact=sin(a*(y-x*x))*sin(a*x)*sin(a*y);
  float src=-4*3.14156*3.14156*sin(3.14156*pt[0]/3) * sin(3.14156*pt[1]/3)/9;
  float exact=-36*src/(8*3.14156*3.14156);
  return new QPoint3f(pt[0],pt[1],exact);   
}


float  QGeometricKernel::area(QAxel::QBSplineSurface * s)
{
  //area computation of bezier surface 
  if(s->ikind==3)
    {
      // create sisl surface
      double points[s->points.size()*3] ;
      for(int i = 0 ; i<s->points.size() ; i++)
	{
	  points[3*i+0] = s->points[i]->x() ;
	  points[3*i+1] = s->points[i]->y() ;
	  points[3*i+2] = s->points[i]->z() ;
	}
      
      SISLSurf *ssurf =  newSurf(s->number1, s->number2, 
				 s->order1,  s->order2,
				 s->knots1,  s->knots2,
				 points, s->ikind, 
				 s->idim, 1) ;
      
      const int seg=20;
      
      float step0 = (s->umax()-s->umin())/(seg-1);
      float step1 = (s->vmax()-s->vmin())/(seg-1);
      
      double parvalue[2];
      int leftknot1;
      int leftknot2;
      double derive[9];
      double normal[3];
      int stat;
      float area=0;

      for(int i= 0; i < seg; i++)
	{	    
	  for(int j= 0; j < seg; j++)
	    {

	      parvalue[0]=s->umin()+i*step0;
	      parvalue[1]=s->vmin()+j*step1;
	      
	      // call to sisl for double precision evaluation
	      s1421(ssurf, 1, parvalue, &leftknot1, &leftknot2, derive, normal, &stat);
	      
	      // first fundemental form of s
	      
	      double e=derive[3]*derive[3]+derive[4]*derive[4]+derive[5]*derive[5];
	      double g=derive[6]*derive[6]+derive[7]*derive[7]+derive[8]*derive[8];
	      double f=derive[3]*derive[6]+derive[4]*derive[7]+derive[5]*derive[8];
	      
	      double element=sqrt(e*g-f*f);
	      area=area+element;
	      
	    }
      
	}
      
      double final=area/(seg*seg);
    
      return final;    
    }
  
  // for spline surface
  else    
    {
      SplineSurface * ss=QBSplineSurfacetoGo(s);
      double tol = 1e-12;
      float area = ss->area(tol);
      return area;
    }
}

//IGA energy for constraint optimization method 
float  QGeometricKernel::IGAEnergy(QAxel::QBSplineSurface * s)
{

 //  //IGA energy computation of bezier surface 
//   if(s->ikind==3)
//     {
      // create sisl surface
      double points[s->points.size()*3] ;
      for(int i = 0 ; i<s->points.size() ; i++)
	{
	  points[3*i+0] = s->points[i]->x() ;
	  points[3*i+1] = s->points[i]->y() ;
	  points[3*i+2] = s->points[i]->z() ;
	}
      
      SISLSurf *ssurf =  newSurf(s->number1, s->number2, 
				 s->order1,  s->order2,
				 s->knots1,  s->knots2,
				 points, s->ikind, 
				 s->idim, 1) ;
      
      const int seg=10;
      
      float step0 = (s->umax()-s->umin())/(seg-1);
      float step1 = (s->vmax()-s->vmin())/(seg-1);
      
      double parvalue[2];
      int leftknot1;
      int leftknot2;
      double derive[18];
      double normal[3];
      int stat;
      float energy=0;
      float w=1;
      //      float a=3.0;
  
      //      float distance=0;


//       regular term for injective parameterization 
//       for(int j=1;j<s->number2-1;j++)
// 	{ 
	  
// 	  for(int i=1;i<s->number1-1;i++)
// 	    {
	      
	      
// 	      float center_x=0.25*s->points[j*s->number1+i-1]->x()+0.25* s->points[j*s->number1+i+1]->x()+0.25*s->points[(j-1)*s->number1+i]->x()+0.25*s->points[(j+1)*s->number1+i]->x(); 
	      
// 	      float center_y=0.25*s->points[j*s->number1+i-1]->y()+0.25* s->points[j*s->number1+i+1]->y()+0.25*s->points[(j-1)*s->number1+i]->y()+0.25*s->points[(j+1)*s->number1+i]->y();     
	      
	    
// 	      QPoint3f* center =new QPoint3f(center_x, center_y, center_z);
	      
// 	      distance=distance+(s->points[j*s->number1+i]->x()-center_x)+(s->points[j*s->number1+i]->y()-center_y); 
	      
// 	    }
	  
	  
// 	}
      

      for(int i= 0; i < seg; i++)
	{

	  parvalue[0]=s->umin()+i*step0;
	    
	  for(int j= 0; j < seg; j++)
	    {

	    
	      parvalue[1]=s->vmin()+j*step1;
	      
	      // call to sisl for double precision evaluation
	      s1421(ssurf, 2, parvalue, &leftknot1, &leftknot2, derive, normal, &stat);
	      
	      // first fundemental form of s
	      
	      //	      double fu=derive[3]*derive[3]+derive[4]*derive[4];
	      //  double fv=derive[6]*derive[6]+derive[7]*derive[7];
	      
	      
              double fufv=derive[3]*derive[6]+derive[4]*derive[7];

              double fuu=derive[9]*derive[9]+derive[10]*derive[10];
	      double fuv=derive[12]*derive[12]+derive[13]*derive[13];
              double fvv=derive[15]*derive[15]+derive[16]*derive[16];
	      
	      //  double Jacobian=derive[3]*derive[7]-derive[4]*derive[6];
	      
	      // double element=(Jacobian-1)+fuu+2*fuv+fvv+w*(fu+fv);
              double element=fuu+4*fuv+fvv+w*(fufv*fufv);
	      energy=energy+element;
	      
	    }
      
	}
       double final=energy/(seg*seg);
       // double final=energy/(seg*seg)+distance/((s->number1-2)*(s->number2-2));
      // double final=distance/((s->number1-2)*(s->number2-2));
      return final;    
//     }
  
//   //for spline surface
//    else    
//     {
//       SplineSurface * ss=QBSplineSurfacetoGo(s);
//       double tol = 1e-12;
     
//       const int seg=10;
      
//       float step0 = (ss->endparam_u()-ss->startparam_u())/(seg-1);
//       float step1 = (ss->endparam_v()-ss->startparam_v())/(seg-1);
      
//       double parvalue[2];
           
//       float energy=0;
//       float w=1.0;
//       float a=100.0;

//       for(int i= 0; i < seg; i++)
// 	{	    
// 	  parvalue[0]=ss->startparam_u()+i*step0;

// 	  for(int j= 0; j < seg; j++)
// 	    {

	     
// 	      parvalue[1]=ss->startparam_v()+j*step1;

//               std::vector<Point> p(6, Point(ss->dimension()));  
// 	      ss->point(p, parvalue[0], parvalue[1], 2);
      
// 	      // first fundemental form of s
	      
// 	      double fus=p[1][0]*p[1][0]+p[1][1]*p[1][1];
// 	      double fvs=p[2][0]*p[2][0]+p[2][1]*p[2][1];

//               double fufvs=p[1][0]*p[2][0]+p[1][1]*p[2][1];
// 	      double fuus=p[3][0]*p[3][0]+p[3][1]*p[3][1];
// 	      double fuvs=p[4][0]*p[4][0]+p[4][1]*p[4][1];
// 	      double fvvs=p[5][0]*p[5][0]+p[5][1]*p[5][1];
	  
//               double Jacobians=p[1][0]*p[2][1]-p[1][1]*p[2][0];       
	       
//               double elements=fuus+4*fuvs+fvvs+w*(fufvs*fufvs);
// 	      //double elements=fuus+2*fuvs+fvvs+w*(fufvs*fufvs);
// 	      // double elements=(fus-fvs)*(fus-fvs)+(fufvs*fufvs);
// 	      energy=energy+elements;
	      
// 	    }
	  
// 	}
      
//       double final=energy/(seg*seg);
    
//       return final;       
      
 
//     }
}

//harmonic energy for elliptic grid generation 
float  QGeometricKernel::harmonicEnergy(QAxel::QBSplineSurface * s)
{

  //harmonic energy computation of bezier surface 
 //  if(s->ikind==3)
//     {
      // create sisl surface
      double points[s->points.size()*3] ;
      for(int i = 0 ; i<s->points.size() ; i++)
	{
	  points[3*i+0] = s->points[i]->x() ;
	  points[3*i+1] = s->points[i]->y() ;
	  points[3*i+2] = s->points[i]->z() ;
	}
      
      SISLSurf *ssurf =  newSurf(s->number1, s->number2, 
				 s->order1,  s->order2,
				 s->knots1,  s->knots2,
				 points, s->ikind, 
				 s->idim, 1) ;
      
      const int seg=10;
      
      float step0 = (s->umax()-s->umin())/(seg-1);
      float step1 = (s->vmax()-s->vmin())/(seg-1);
      
      double parvalue[2];
      int leftknot1;
      int leftknot2;
      double derive[18];
      double normal[3];
      int stat;
      float energy=0;
      float w=5.0;

      for(int i= 0; i < seg; i++)
	{
	  parvalue[0]=s->umin()+i*step0;
	    
	  for(int j= 0; j < seg; j++)
	    {

	    
	      parvalue[1]=s->vmin()+j*step1;
	      
	      // call to sisl for double precision evaluation
	      s1421(ssurf, 2, parvalue, &leftknot1, &leftknot2, derive, normal, &stat);
	      
	      // first fundemental form of s
	      
	      double fu=derive[3]*derive[3]+derive[4]*derive[4];
	      double fv=derive[6]*derive[6]+derive[7]*derive[7];
	      
	      
              double fufv=derive[3]*derive[6]+derive[4]*derive[7];

              double fuu=derive[9]*derive[9]+derive[10]*derive[10];
	      double fuv=derive[12]*derive[12]+derive[13]*derive[13];
              double fvv=derive[15]*derive[15]+derive[16]*derive[16];

	      // double Jacobian=derive[3]*derive[7]-derive[4]*derive[6];

              double px=fu*derive[9]-2*fufv*derive[12]+fv*derive[15];
	      double py=fu*derive[10]-2*fufv*derive[13]+fv*derive[16];


	      double element=20*fuu+16*fuv+20*fvv+w*(fufv*fufv);  
	     
	      // double element=px*px+py*py+50*element1;

	      energy=energy+element;
	      
	    }
      
	}
      
      double final=energy/(seg*seg);
    
      return final;    
      //   }
  
//   //for spline surface
//    else    
//     {
//       SplineSurface * ss=QBSplineSurfacetoGo(s);
//       double tol = 1e-12;
     
//       const int seg=10;
      
//       float step0 = (ss->endparam_u()-ss->startparam_u())/(seg-1);
//       float step1 = (ss->endparam_v()-ss->startparam_v())/(seg-1);
      
//       double parvalue[2];
           
//       float energy=0;

//       for(int i= 0; i < seg; i++)
// 	{	    

//             parvalue[0]=ss->startparam_u()+i*step0;

// 	  for(int j= 0; j < seg; j++)
// 	    {
	      
// 	      parvalue[1]=ss->startparam_v()+j*step1;
	      
//               std::vector<Point> p(6, Point(ss->dimension()));  
// 	      ss->point(p, parvalue[0], parvalue[1], 2);
      
// 	      // first fundemental form of s
	      
// 	      double fus=p[1][0]*p[1][0]+p[1][1]*p[1][1];
// 	      double fvs=p[2][0]*p[2][0]+p[2][1]*p[2][1];

//               double fufvs=p[1][0]*p[2][0]+p[1][1]*p[2][1];
// 	      double fuus=p[3][0]*p[3][0]+p[3][1]*p[3][1];
// 	      double fuvs=p[4][0]*p[4][0]+p[4][1]*p[4][1];
// 	      double fvvs=p[5][0]*p[5][0]+p[5][1]*p[5][1];

// 	      double pxs=fus*p[3][0]-2*fufvs*p[4][0]+fvs*p[5][0];
// 	      double pys=fus*p[3][1]-2*fufvs*p[4][1]+fvs*p[5][1];
	       
//               double Jacobians=p[1][0]*p[2][1]-p[1][1]*p[2][0];     
	      
// 	      // double elements=pxs*pxs+pys*pys+1000*(pxs-pys)*(pxs-pys);
	      
// 	      double elements=pxs*pxs+pys*pys;
	      
// 	      energy=energy+elements;
	      
// 	    }
      
// 	}
      
//       double final=energy/(seg*seg);   
//       return final;       
  
//     }
}




float  QGeometricKernel::meanCurvature(QBSplineSurface * s, float u, float v) 
{
   
  SplineSurface * ss=QBSplineSurfacetoGo(s);
  double K;
  double H;
  curvatures(*ss, u, v, K, H);
  if(H<0)
  H=-H;
  return H;
}

float  QGeometricKernel::GaussianCurvature(QBSplineSurface * s, float u, float v) 
{
 
  SplineSurface * ss=QBSplineSurfacetoGo(s);
  double K;
  double H;
  curvatures(*ss, u, v, K, H);
  return K;
 
} 

 
 

void QGeometricKernel::draw_mean(QMeanCurvatureColorMap * s)
{
 SplineSurface * ss=QBSplineSurfacetoGo(s->surf);

 int m = 50;
 int n = 50;
 IGLGood * result = new IGLGood(IGLGood::E_QUAD,m*n,4*(m-1)*(n-1),true) ;

 //filled vertices and colors
 
 hsv_colormap.set_value_range(s->Hmax, s->Hmin);
 
 float mc[3];
 
 for(int i = 0 ; i < m ; i++) {
   float u = ss->startparam_u() +i*(ss->endparam_u()-ss->startparam_u())/(m-1) ;
   for(int j = 0 ; j < n ; j++) {
     float v = ss->startparam_v() +j*(ss->endparam_v()-ss->startparam_v())/(n-1) ;
     
     Go::Point pt1;
     ss->point(pt1,u,v);
     double K;
     double H;
     curvatures(*ss, u, v, K, H);
     getcolor(H,mc);
     result->colors[3*m*i+3*j]=mc[0];
     result->colors[3*m*i+3*j+1]=mc[1];
     result->colors[3*m*i+3*j+2]=mc[2];
     result->vertices[3*m*i+3*j]=pt1[0];
     result->vertices[3*m*i+3*j+1]=pt1[1];
     result->vertices[3*m*i+3*j+2]=pt1[2];
     
     Go::Point pt2;
     ss->normal(pt2,u,v);
     result->normals[3*m*i+3*j]=pt2[0];
     result->normals[3*m*i+3*j+1]=pt2[1];
     result->normals[3*m*i+3*j+2]=pt2[2];
    }
  }
  
  int c = 0 ;
  for(int i = 0 ; i < m-1; i ++)
    for(int j = 0 ; j < n-1; j ++, c += 4) {
      int a = i*n+j ;
      result->indexes[c] = a ;
      result->indexes[c+1] = a + n ;
      result->indexes[c+2] = a + n + 1 ;
      result->indexes[c+3] = a + 1 ;
    }
  // cout << "filled indexes" << endl ;
   
 
  glEnableClientState(GL_VERTEX_ARRAY) ;
  glVertexPointer(3, GL_DOUBLE, 0, result->vertices) ;
  
  glEnableClientState(GL_COLOR_ARRAY) ;   
  glColorPointer(3,GL_FLOAT, 0, result->colors) ; 
  
  glEnableClientState(GL_NORMAL_ARRAY) ;   
  glNormalPointer(GL_DOUBLE, 0, result->normals) ;
  
  glDisable(GL_LIGHTING) ;
  glDrawElements(GL_QUADS, result->nbi, GL_UNSIGNED_INT, result->indexes) ;
  glEnable(GL_LIGHTING) ;
  glEnable(GL_COLOR_MATERIAL);

  glDisableClientState(GL_VERTEX_ARRAY) ;
  glDisableClientState(GL_COLOR_ARRAY) ;
  glDisableClientState(GL_NORMAL_ARRAY) ;
}



//image editing by texure surface method

QString image_path;

GLuint	texture[1];

void loadGLTextures()
{
  QImage t;
  QImage b;

  //define image input path
  image_path = qApp->applicationDirPath() + "/../../toolbox/axel/bin/images/cow.bmp";
  
  //#if defined (Q_WS_MAC)
  //image_path = qApp->applicationDirPath() + "/../.././images/gxu.bmp";
  //#else
  // image_path = qApp->applicationDirPath() + "/../../toolbox/axel/bin/images/gxu.bmp";
  //#endif  
  
  //use QImage for image loading 
  if (!b.load(image_path)) {
    qWarning( "Could not read image file" );
    QImage dummy( 128, 128, QImage::Format_RGB32 );
    b = dummy;
  }
  
  t = QGLWidget::convertToGLFormat( b );
  
  //generate texture mapping
  glGenTextures( 1, &texture[0] );
 
  glBindTexture(GL_TEXTURE_2D, texture[0] );
  glTexImage2D(GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );
  
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
}

void QGeometricKernel::draw(QTextureSurface * s)
{ 
  int x, y;
  float float_x, float_y, float_xb, float_yb;
  
  //initial opengl variables
  loadGLTextures();
  glEnable(GL_TEXTURE_2D);
  glShadeModel(GL_SMOOTH);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glPolygonMode( GL_BACK, GL_FILL );

  //reset lighting conditions 

  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  
  GLfloat mat_shininess[] = { 100.0 };
  
  GLfloat light_position_1[] = { 1.0, 1.0, 3.0, 0.0 };
  
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  
  glLightfv(GL_LIGHT0, GL_POSITION, light_position_1);
  
  GLfloat light_position_2[] = { 3.0, 3.0, 3.0, 0.0 };

  glLightfv(GL_LIGHT1, GL_POSITION, light_position_2);
  
  glEnable(GL_LIGHTING);
  
  glEnable(GL_LIGHT0);
  
  glEnable(GL_LIGHT1);
  //use gotools for evaluation 
  SplineSurface * ss=QBSplineSurfacetoGo(s->surf);
  
  const int seg =300;
  double step0 = (ss->endparam_u()-ss->startparam_u())/(seg-1);
  double step1 = (ss->endparam_v()-ss->startparam_v())/(seg-1);
 
  float points[seg+1][seg+1][3];
 
  Go::Point pt1;

  for(int x=0; x<seg; x++)
    {
      for(int y=0; y<seg; y++)
	{
	
	  ss->point(pt1,ss->startparam_u()+y*step0,ss->startparam_v()+x*step1);   
	  
	  points[x][y][0]=float(pt1[0]);
	  points[x][y][1]=float(pt1[1]);
	  points[x][y][2]=float(pt1[2]);
	}
    }
  

  //draw textures (for better illustration, change the loop for x and y)
  glBindTexture(GL_TEXTURE_2D, texture[0]);
  
  glBegin(GL_QUADS);
  for( y = 0; y < (seg-1); y++ )
    {
      for( x = 0; x < (seg-1); x++ )
	{
	  float_x = float(x)/float(seg-1);
	  float_y = float(y)/float(seg-1);
	  float_xb = float(x+1)/float(seg-1);
	  float_yb = float(y+1)/float(seg-1);
	  
	  glTexCoord2f( float_x, float_y);
	  glVertex3f( points[x][y][0], points[x][y][1], points[x][y][2] );
	  glTexCoord2f( float_x, float_yb );
	  glVertex3f( points[x][y+1][0], points[x][y+1][1], points[x][y+1][2]);
	  glTexCoord2f( float_xb, float_yb );
	  glVertex3f( points[x+1][y+1][0], points[x+1][y+1][1], points[x+1][y+1][2] );
	  glTexCoord2f( float_xb, float_y );
	  glVertex3f( points[x+1][y][0], points[x+1][y][1], points[x+1][y][2] );
	}
    }
  glEnd();
  glDisable(GL_TEXTURE_2D);
}



int factorial (int num)
{
  if (num==1)
  return 1;
  return factorial(num-1)*num; // recursive call
}


float Pn(int n, float u, float v)
{
  
  float Pn=0.0; 

  int pn ;  

  if((n-1)%2==0)
    {
      pn = (n-1)/2;     
    }
  else
    {
      pn = n/2;      
    }  
  
  
  for(int k=0; k<pn; k++)
    {
      int coef1=1;
      float pn1=0.0;
 
      if(k==0)
	{
        pn1=pow(u,n-2*k)*pow(v,2*k);
	}
      else
	coef1=factorial(n)/((factorial(n-2*k)*factorial(2*k)));   
      int sign =1;       
      
      if(k%2==0)
	sign=1;
      else
	sign=-1; 
      
      pn1=sign*coef1*pow(u,n-2*k)*pow(v,2*k);
      Pn=Pn+pn1; 
      
    }    

  return Pn; 
}


float Qn(int n, float u, float v)
{
  
  float Qn=0.0; 

  int qn;  

  if((n-1)%2==0)
    {
      qn = (n-1)/2;   
    }
  else
    { 
      qn =(n-2)/2;
    }  
  
  for(int k=0; k<qn; k++)
    {
      int coef2;
      if(k==0)
        coef2=1;
      else
	coef2=factorial(n)/((factorial(n-2*k-1)*factorial(2*k+1)));   
      int sign;       

      if(k%2==0)
	sign=1;
      else
	sign=-1; 
      
      float qn1=sign*coef2*pow(u,n-2*k-1)*pow(v,2*k+1);
      Qn=Qn+qn1; 
      
    }    
  
  return Qn; 
}

void QGeometricKernel::draw_mini(QGeneralMiniSurface* s)
{
  
  
  int m = 100;
  int n = 100;
  IGLGood * result = new IGLGood(IGLGood::E_QUAD,m*n,4*(m-1)*(n-1),true) ;
  
  //filled vertices 
  float zcoeff=(2*sqrt(s->degree*(s->degree-2)*s->omega)*1.0/(s->degree-1)*1.0);
 
  for(int i = 0 ; i < m ; i++) {
    float u = s->umin + i*(s->umax-s->umin)/(m-1) ;
    for(int j = 0 ; j < n ; j++) {
      float v = s->vmin + j*( s->vmax-s->vmin)/(n-1) ;

      float pn1value=Pn(s->degree,u,v);
      float pn2value=Pn(s->degree-2,u,v); 
 
      float x=-pn1value+s->omega*pn2value;
      
      float qn1value=Qn(s->degree,u,v);
      float qn2value=Qn(s->degree-2,u,v); 
 
      float y=qn1value+s->omega*qn2value;


      float pn3value=Pn(s->degree-1,u,v); 
      float z=zcoeff*pn3value;

      result->vertices[3*m*i+3*j]=x;
      result->vertices[3*m*i+3*j+1]=y;
      result->vertices[3*m*i+3*j+2]=z;
      
    }
  }

  fxv<double,3> * normals = (fxv<double, 3>*)result->normals ;
  const fxv<double,3> * smp = (fxv<double, 3>*)result->vertices ;
  
  int _N[m]; std::fill(_N,_N+m,-n);  _N[0]   = 0 ;
  int _S[m]; std::fill(_S,_S+m,n);   _S[m-1] = 0 ;
  int _E[n]; std::fill(_E,_E+n,1);   _E[n-1] = 0 ;
  int _W[n]; std::fill(_W,_W+n,-1);  _W[0]   = 0 ;	
  int *N,*S,*E,*W ;
  
  for(N = _N, S = _S; N != _N + m; N++, S++)
    for(E = _E, W = _W; E != _E + n; E++, W++,  normals++, smp ++) {
      fxv<double,3> du,dv;
      sub(du,*(smp+*S),*(smp+*N));
      //  scmul(du,9.0/2.0);                                                                                                                                                                        
      sub(dv,*(smp+*E),*(smp+*W));
      //  scmul(dv,9.0/2.0);                                                                                                                                                                        
      crossprod(*normals,du,dv);
      div(*normals,sqrt(dotprod(*normals,*normals)));
    }
  
  int c = 0 ;
  for(int i = 0 ; i < m-1; i ++)
    for(int j = 0 ; j < n-1; j ++, c += 4) {
      int a = i*n+j ;
      result->indexes[c] = a ;
      result->indexes[c+1] = a + n ;
      result->indexes[c+2] = a + n + 1 ;
      result->indexes[c+3] = a + 1 ;
    }
  // cout << "filled indexes" << endl ;
   
 
  glEnableClientState(GL_VERTEX_ARRAY) ;
  glVertexPointer(3, GL_DOUBLE, 0, result->vertices) ;
  
  glEnableClientState(GL_NORMAL_ARRAY) ;   
  glNormalPointer(GL_DOUBLE, 0, result->normals) ;
  
  glDisable(GL_LIGHTING) ;
  glDrawElements(GL_QUADS, result->nbi, GL_UNSIGNED_INT, result->indexes) ;
  glEnable(GL_LIGHTING) ;
  
  glDisableClientState(GL_VERTEX_ARRAY) ;
  glDisableClientState(GL_NORMAL_ARRAY) ;
   
}


 
# ifndef Q_OS_WIN
Q_EXPORT_PLUGIN(QGeometricKernel)
# endif
