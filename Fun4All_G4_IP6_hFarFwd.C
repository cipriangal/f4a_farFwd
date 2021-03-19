#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include "GlobalVariables.C"
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDummyInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <g4histos/G4HitNtuple.h>
#include <g4eval/PHG4DSTReader.h>
#include <g4main/PHG4ParticleGun.h>
#include <g4main/HepMCNodeReader.h>
#include <g4main/ReadEICFiles.h>
#include <g4main/PHG4Reco.h>
#include <g4main/PHG4TruthSubsystem.h>
#include <g4detectors/PHG4CylinderSubsystem.h>
#include <g4detectors/PHG4ConeSubsystem.h>
#include <g4detectors/BeamLineMagnetSubsystem.h>
#include <g4detectors/PHG4BlockSubsystem.h>
#include <phool/recoConsts.h>

#include <fun4all_compton/ComptonTruthSubsystem.h>
#include <fun4all_compton/ComptonIO.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4eval.so)
R__LOAD_LIBRARY(libg4detectors.so)
R__LOAD_LIBRARY(libg4histos.so)
R__LOAD_LIBRARY(libCompton.so)

#endif

#include <set>

void defineDetectors(PHG4Reco*,int);
void defineBeamPipe(PHG4Reco*);

//bool verbose = true;
bool verbose = false;

void Fun4All_G4_IP6_hFarFwd(
			    int nEvents = -1, 
			    const std::string foutNm="o_ComptonTst.root"
			    )
{

  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4testbench.so");  
  gSystem->Load("libg4histos");
  gSystem->Load("libg4eval");
  gSystem->Load("libCompton.so");

  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  if(nEvents<5 && nEvents>0)
    se->Verbosity(2);
  recoConsts *rc = recoConsts::instance();

  PHG4ParticleGun *gun = new PHG4ParticleGun();
  gun->set_pid(2212);
  //gun->set_name("chargedgeantino");//positive charge!!
  TVector3 gMom(0,0,250);
  gMom.RotateY(0.025);
  gun->set_vtx(0, 0, 0);
  gun->set_mom(gMom.X(), gMom.Y(), gMom.Z());

  if(nEvents<5)
    cout<<"Momentum "<<gMom.X()<<" "<<gMom.Y()<<" "<<gMom.Z()<<endl;
  se->registerSubsystem(gun);

  // make magnet active volume if you want to study the hits
  bool magnet_active=false;
  int absorberactive = 0;

  // if you insert numbers it only displays those magnets, do not comment out the set declaration
  set<int> magnetlist;
  //magnetlist.insert(7);
  //  magnetlist.insert(3);

  //
  // Geant4 setup
  //
  PHG4Reco* g4Reco = new PHG4Reco();
  if(nEvents<5 && verbose && nEvents>0)
    g4Reco->Verbosity(2);

  // setup of G4: 
  //   no saving of geometry: it takes time and we do not do tracking
  //   so we do not need the geometry
  g4Reco->save_DST_geometry(false);
  g4Reco->set_field(0);
  g4Reco->SetWorldMaterial("G4_Galactic");

  BeamLineMagnetSubsystem *bl = nullptr;
  std::ifstream infile("ip6_h_farFwdBeamline.dat");
  if (infile.is_open())
    {
      double biggest_z = 0.;
      int imagnet = 0;
      std::string line;
      while (std::getline(infile, line))
	{
	  //	cout << line << endl;
	  if (! line.compare(0,1,"B") || 
	      ! line.compare(0,1,"Q") ||
	      ! line.compare(0,1,"S"))
	    {
	      std::istringstream iss(line);
	      string magname;
	      double x;
	      double y;
	      double z;
	      double inner_radius_zin;
	      double inner_radius_zout;
	      double outer_magnet_diameter;
	      double length;
	      double angle;
	      double dipole_field_x;
	      double fieldgradient;
	      if (!(iss >> magname >> x >> y >> z 
		    >> inner_radius_zin >> inner_radius_zout
		    >> outer_magnet_diameter >> length
		    >> angle >> dipole_field_x >> fieldgradient))
		{
		  cout << "coud not decode " << line << endl;
		  gSystem->Exit(1);
		}
	      else
		{
		  string magtype;
		  if (inner_radius_zin != inner_radius_zout)
		    {
		      cout << "inner radius at front of magnet " << inner_radius_zin
			   << " not equal radius at back of magnet " << inner_radius_zout
			   << " needs change in code (replace tube by cone for beamline)" << endl;
		      gSystem->Exit(1);
		    }
		  if(verbose  || (nEvents<5 && nEvents>0)){
		    cout << endl << endl << "\tID number "<<imagnet<<endl;
		    cout << "magname: " << magname << endl;
		    cout << "x: " << x << endl;
		    cout << "y: " << y << endl;
		    cout << "z: " << z << endl;
		    cout << "inner_radius_zin: " << inner_radius_zin << endl;
		    cout << "inner_radius_zout: " << inner_radius_zout << endl;
		    cout << "outer_magnet_diameter: " << outer_magnet_diameter << endl;
		    cout << "length: " << length << endl;
		    cout << "angle: " << angle << endl;
		    cout << "dipole_field_x: " << dipole_field_x << endl;
		    cout << "fieldgradient: " << fieldgradient << endl;
		  }
		  if (! magname.compare(0,1,"B")){
		    magtype = "DIPOLE";
		  }else if (! magname.compare(0,1,"Q")){
		    magtype = "QUADRUPOLE";
		  }else if (! magname.compare(0,1,"S")){
		    magtype = "SEXTUPOLE";
		  }else{
		    cout << "cannot decode magnet name " << magname << endl;
		    gSystem->Exit(1);
		  }
		  // convert to our units (cm, deg)
		  x *= 100.;
		  y *= 100.;
		  z *= 100.;
		  length *= 100.;
		  inner_radius_zin *= 100.;
		  outer_magnet_diameter *= 100.;
		  angle = (angle/TMath::Pi()*180.)/1000.; // given in mrad

		  // dipole_field_x *= gFactor;
		  // fieldgradient *= gFactor;//FIXME xcheck if you need this

		  if (magnetlist.empty() || magnetlist.find(imagnet) != magnetlist.end())
		    {
		      //cout<<magname<<" "<<imagnet<<" "<<angle<<endl;
		      bl = new BeamLineMagnetSubsystem("BEAMLINEMAGNET",imagnet);
		      bl->set_double_param("field_y",dipole_field_x);
		      bl->set_double_param("field_x",0.);
		      bl->set_double_param("fieldgradient",fieldgradient);
		      bl->set_string_param("magtype",magtype);
		      bl->set_double_param("length",length);
		      bl->set_double_param("place_x",x);
		      bl->set_double_param("place_y",y);
		      bl->set_double_param("place_z",z);
		      bl->set_double_param("rot_y",angle);
		      bl->set_double_param("inner_radius",inner_radius_zin);
		      bl->set_double_param("outer_radius", outer_magnet_diameter/2.);
		      bl->SetActive(magnet_active);
		      //bl->BlackHole();
		      if (absorberactive)  
			{
			  bl->SetAbsorberActive();
			}
		      bl->OverlapCheck(overlapcheck);
		      bl->SuperDetector("BEAMLINEMAGNET");
		      g4Reco->registerSubsystem(bl);
		    }
		  imagnet++;
		  if (fabs(z)+length > biggest_z)
		    {
		      biggest_z = fabs(z)+length;
		    }
		}
	    }
	}
      infile.close();
      if (biggest_z*2. > g4Reco->GetWorldSizeZ())
	{
	  g4Reco->SetWorldSizeZ((biggest_z+100.)*2); // leave 1m on both sides
	}
    }
  g4Reco->SetWorldSizeZ(4500*2); //in cm

  //1=primaries only (trackID 1,2); 0=all particles
  const int trackingLevel = 1;
  defineDetectors(g4Reco, trackingLevel);

  defineBeamPipe(g4Reco);

  if(verbose || nEvents<5)
    cout<<"World size: "<<g4Reco->GetWorldSizeX()<<" "<<g4Reco->GetWorldSizeY()<<" "<<g4Reco->GetWorldSizeZ()<<" "<<endl;

  se->registerSubsystem(g4Reco);

  if (nEvents>0){
    PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
    g4Reco->registerSubsystem(truth);

    ComptonIO *cmtOut = new ComptonIO("ComptonIO",foutNm);
    if(verbose || (nEvents<5 && nEvents>0))
      cmtOut->Verbosity(4);
    cmtOut->AddNode("zdc",0,0);
    se->registerSubsystem(cmtOut);
  }

  // this (dummy) input manager just drives the event loop
  Fun4AllInputManager *in = new Fun4AllDummyInputManager( "Dummy");
  se->registerInputManager( in );

  if (nEvents <= 0)
    {
      return 0;
    }
  se->run(nEvents);
  se->End();
  delete se;
  gSystem->Exit(0);
}

void defineBeamPipe(PHG4Reco* g4Reco){
  //exit window
  PHG4CylinderSubsystem *exitWin = new PHG4CylinderSubsystem("exitWin",0);
  exitWin->set_double_param("radius"   ,3.2);
  exitWin->set_double_param("thickness",11.8);
  exitWin->set_double_param("length"   ,0.15);
  exitWin->set_double_param("rot_y"    ,-0.025*TMath::RadToDeg());
  exitWin->set_string_param("material" ,"G4_STAINLESS-STEEL");
  exitWin->set_double_param("place_x"  ,12.5);
  exitWin->set_double_param("place_y"  ,0);
  exitWin->set_double_param("place_z"  ,500);
  exitWin->SetActive(false);
  g4Reco->registerSubsystem(exitWin);

  //B0 magnet pipe
  PHG4CylinderSubsystem *pipeB0 = new PHG4CylinderSubsystem("beamPipeB0",0);
  pipeB0->set_double_param("radius",2.8);
  pipeB0->set_double_param("thickness",0.25);
  pipeB0->set_double_param("length",195);
  pipeB0->set_double_param("rot_y",-0.025*TMath::RadToDeg());
  pipeB0->set_string_param("material","G4_Al");
  pipeB0->set_double_param("place_x",14.748);
  pipeB0->set_double_param("place_y",0);
  pipeB0->set_double_param("place_z",590);
  pipeB0->SetActive(false);
  g4Reco->registerSubsystem(pipeB0);

  //Quad pipes
  const int nSecQ = 5; //B0apf, Q1apf, Q1bpf, Q2pf, B1pf
  const string nm  [nSecQ]={"B0apf", "Q1apf", "Q1bpf", "Q2pf", "B1pf"};
  const double qlen[nSecQ]={160    , 150    , 220    , 440   , 330   };
  const double qir [nSecQ]={4      , 5.1    , 7      , 12    , 12.2  };
  const double qor [nSecQ]={4.1    , 5.2    , 7.2    , 12.2  , 12.4  };
  const double qrot[nSecQ]={25     , 19.5   , 15     , 15    , 34    };//mrad
  const double qxC [nSecQ]={19.8   , 24.47  , 30.05  , 39.5  , 48    };
  const double qyC [nSecQ]={0      , 0      , 0      , 0     , 0     };
  const double qzC [nSecQ]={770    , 922.8  , 1106.3 , 1416.7, 1806.7};
  for(int i=0;i<nSecQ;i++){
    PHG4CylinderSubsystem *pipe = new PHG4CylinderSubsystem(Form("beamPipe%s",nm[i].c_str()),0);
    pipe->set_double_param("radius",qir[i]);
    pipe->set_double_param("thickness",qor[i]-qir[i]);
    pipe->set_double_param("length",qlen[i]);
    pipe->set_double_param("rot_y",-qrot[i]/1000*TMath::RadToDeg());
    pipe->set_string_param("material","G4_Al");
    pipe->set_double_param("place_x",qxC[i]);
    pipe->set_double_param("place_y",qyC[i]);
    pipe->set_double_param("place_z",qzC[i]);
    pipe->SetActive(false);
    g4Reco->registerSubsystem(pipe);
  }

  //Electron pipe
  PHG4CylinderSubsystem *pipeElectron = new PHG4CylinderSubsystem("beamPipeElectron",0);
  pipeElectron->set_double_param("radius",1);
  pipeElectron->set_double_param("thickness",1);
  pipeElectron->set_double_param("length",3000);
  pipeElectron->set_double_param("rot_y",-0.025*TMath::RadToDeg());
  pipeElectron->set_string_param("material","G4_Al");
  pipeElectron->set_double_param("place_x",0);
  pipeElectron->set_double_param("place_y",0);
  pipeElectron->set_double_param("place_z",2000);
  pipeElectron->SetActive(false);
  //g4Reco->registerSubsystem(pipeElectron);

  //ZDC pipe
  PHG4CylinderSubsystem *pipeZDC = new PHG4CylinderSubsystem("beamPipeZDC",0);
  pipeZDC->set_double_param("radius",16.5);
  pipeZDC->set_double_param("thickness",0.1);
  pipeZDC->set_double_param("length",170);
  pipeZDC->set_double_param("rot_y",-0.025*TMath::RadToDeg());
  pipeZDC->set_string_param("material","G4_Al");
  pipeZDC->set_double_param("place_x",59);
  pipeZDC->set_double_param("place_y",0);
  pipeZDC->set_double_param("place_z",2041.59);
  pipeZDC->SetActive(false);
  g4Reco->registerSubsystem(pipeZDC);

  //Roman Pot pipe
  const int nSec = 2;
  const double len[nSec]={850,1150 };
  const double ir1[nSec]={17  , 17 };
  const double or1[nSec]={17.1,17.1};
  const double ir2[nSec]={17  ,  7 };
  const double or2[nSec]={17.1, 7.1};
  const double xC[nSec] ={83  , 130};
  const double yC[nSec] ={0   ,   0};
  const double zC[nSec] ={2550,3550};
  for(int i=0;i<nSec;i++){
    PHG4ConeSubsystem *pipe = new PHG4ConeSubsystem(Form("beamPipeRP%d",i),0);
    pipe->set_string_param("material","G4_STAINLESS-STEEL");
    pipe->set_double_param("place_x",xC[i]);
    pipe->set_double_param("place_y",yC[i]);
    pipe->set_double_param("place_z",zC[i]);
    pipe->set_double_param("length",len[i]/2);
    pipe->set_double_param("rmin1",ir1[i]);
    pipe->set_double_param("rmin2",ir2[i]);
    pipe->set_double_param("rmax1",or1[i]);
    pipe->set_double_param("rmax2",or2[i]);
    pipe->set_double_param("rot_y",-0.027*TMath::RadToDeg());
    g4Reco->registerSubsystem(pipe);
  }
}

void defineDetectors(PHG4Reco* g4Reco, int trackLvl){

  auto *detZDC = new ComptonTruthSubsystem("zdc");
  detZDC->set_double_param("place_x",96.24);
  detZDC->set_double_param("place_y",0);
  detZDC->set_double_param("place_z",3800-50);
  detZDC->set_double_param("size_x",60);
  detZDC->set_double_param("size_y",60);
  detZDC->set_double_param("size_z",0.1);
  detZDC->set_string_param("material","G4_Galactic");
  detZDC->SetActive();
  if(verbose)
    detZDC->Verbosity(4);
  detZDC->SetTrackingLevel(trackLvl);
  g4Reco->registerSubsystem(detZDC);


  /*
  //Simple flat detector
  auto *genDet = new ComptonTruthSubsystem("gen");
  genDet->set_double_param("place_x",0);
  genDet->set_double_param("place_y",0);
  genDet->set_double_param("place_z",-5);
  genDet->set_double_param("size_x",100);
  genDet->set_double_param("size_y",100);
  genDet->set_double_param("size_z",0.1);
  genDet->set_string_param("material","G4_Galactic");
  genDet->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    genDet->Verbosity(4);
  genDet->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(genDet);

  auto *dipoleExitDet = new ComptonTruthSubsystem("dExit");
  dipoleExitDet->set_double_param("place_x",0);
  dipoleExitDet->set_double_param("place_y",0);
  dipoleExitDet->set_double_param("place_z",-500);
  dipoleExitDet->set_double_param("size_x",100);
  dipoleExitDet->set_double_param("size_y",100);
  dipoleExitDet->set_double_param("size_z",0.1);
  dipoleExitDet->set_string_param("material","G4_Galactic");
  dipoleExitDet->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    dipoleExitDet->Verbosity(4);
  dipoleExitDet->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(dipoleExitDet);

  auto *qf11ExitDet = new ComptonTruthSubsystem("qf11Exit");
  qf11ExitDet->set_double_param("place_x",0);
  qf11ExitDet->set_double_param("place_y",0);
  qf11ExitDet->set_double_param("place_z",-650);
  qf11ExitDet->set_double_param("size_x",100);
  qf11ExitDet->set_double_param("size_y",100);
  qf11ExitDet->set_double_param("size_z",0.1);
  qf11ExitDet->set_string_param("material","G4_Galactic");
  qf11ExitDet->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    qf11ExitDet->Verbosity(4);
  qf11ExitDet->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(qf11ExitDet);

  auto *qd10EntranceDet = new ComptonTruthSubsystem("qd10Enter");
  qd10EntranceDet->set_double_param("place_x",0);
  qd10EntranceDet->set_double_param("place_y",0);
  qd10EntranceDet->set_double_param("place_z",-1600);
  qd10EntranceDet->set_double_param("size_x",100);
  qd10EntranceDet->set_double_param("size_y",100);
  qd10EntranceDet->set_double_param("size_z",0.1);
  qd10EntranceDet->set_string_param("material","G4_Galactic");
  qd10EntranceDet->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    qd10EntranceDet->Verbosity(4);
  qd10EntranceDet->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(qd10EntranceDet);

  auto *qd10ExitDet = new ComptonTruthSubsystem("qd10Exit");
  qd10ExitDet->set_double_param("place_x",0);
  qd10ExitDet->set_double_param("place_y",0);
  qd10ExitDet->set_double_param("place_z",-1670);
  qd10ExitDet->set_double_param("size_x",100);
  qd10ExitDet->set_double_param("size_y",100);
  qd10ExitDet->set_double_param("size_z",0.1);
  qd10ExitDet->set_string_param("material","G4_Galactic");
  qd10ExitDet->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    qd10ExitDet->Verbosity(4);
  qd10ExitDet->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(qd10ExitDet);

  auto *det25m = new ComptonTruthSubsystem("det25m");
  det25m->set_double_param("place_x",0);
  det25m->set_double_param("place_y",0);
  det25m->set_double_param("place_z",-2500);
  det25m->set_double_param("size_x",100);
  det25m->set_double_param("size_y",100);
  det25m->set_double_param("size_z",0.1);
  det25m->set_string_param("material","G4_Galactic");
  det25m->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    det25m->Verbosity(4);
  det25m->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(det25m);

  auto *qf9EntranceDet = new ComptonTruthSubsystem("qf9Enter");
  qf9EntranceDet->set_double_param("place_x",0);
  qf9EntranceDet->set_double_param("place_y",0);
  qf9EntranceDet->set_double_param("place_z",-2700);
  qf9EntranceDet->set_double_param("size_x",100);
  qf9EntranceDet->set_double_param("size_y",100);
  qf9EntranceDet->set_double_param("size_z",0.1);
  qf9EntranceDet->set_string_param("material","G4_Galactic");
  qf9EntranceDet->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    qf9EntranceDet->Verbosity(4);
  qf9EntranceDet->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(qf9EntranceDet);

  auto *qf9ExitDet = new ComptonTruthSubsystem("qf9Exit");
  qf9ExitDet->set_double_param("place_x",0);
  qf9ExitDet->set_double_param("place_y",0);
  qf9ExitDet->set_double_param("place_z",-2765);
  qf9ExitDet->set_double_param("size_x",100);
  qf9ExitDet->set_double_param("size_y",100);
  qf9ExitDet->set_double_param("size_z",0.1);
  qf9ExitDet->set_string_param("material","G4_Galactic");
  qf9ExitDet->SetActive();
  if(verbose  || (nEvents<5 && nEvents>0))
    qf9ExitDet->Verbosity(4);
  qf9ExitDet->SetTrackingLevel(trackingLevel);
  g4Reco->registerSubsystem(qf9ExitDet);
  */
}
