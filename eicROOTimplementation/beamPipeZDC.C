
void beamPipeZDC()
{
  // Load basic libraries;
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  EicGeoParData *beamPipe = new EicGeoParData("BEAMPIPEZDC", 0, 0);
  beamPipe->SetFileName("beampipeZDC.root");

  // Do not care much about precise description;
  Double_t length            =  1700; //2300        //17500.0;//16500; //23212.8;
  Double_t inRadius          = 165.0; //168.0; //inner radius
  Double_t outRadius         = 166.0; //170.0; //outer radius
  Double_t rotationAngle           =  100.0;
  Double_t xCenter =         590.000; 
  Double_t yCenter =         0.0;
  Double_t zCenter =        20415.9; //30315.0;
  
  TGeoRotation *rw = new TGeoRotation();
  double angle = .025*TMath::RadToDeg(); //.041
  
  rw->RotateY(angle);

  TGeoTube *pipe = new TGeoTube("ZDCBeamPipe", 0.1*inRadius, 0.1 * outRadius, 0.1 * length/2);
  //TGeoVolume *vpipe = new TGeoVolume("ZDCBeamPipe", pipe, beamPipe->GetMedium("beryllium"));
  TGeoVolume *vpipe = new TGeoVolume("ZDCBeamPipe", pipe, beamPipe->GetMedium("aluminum"));
  //TGeoVolume *vpipe = new TGeoVolume("ZDCBeamPipe", pipe, beamPipe->GetMedium("StainlessSteel"));  

  beamPipe->GetTopVolume()->AddNode(vpipe, 0, new TGeoCombiTrans(0.1 * xCenter,0.1 *  yCenter, 0.1 * zCenter, rw));
  
  EicGeoMap *fgmap = beamPipe->CreateNewMap();
  fgmap->AddGeantVolumeLevel("ZDCBeamPipe",1);
  fgmap->SetSingleSensorContainerVolume("ZDCBeamPipe");
    
  beamPipe->AddLogicalVolumeGroup(0, 0, 1);//waferNum);

  UInt_t geant[1] = {0}, group = 0, logical[3] = {0, 0, 0};
    
  if (beamPipe->SetMappingTableEntry(fgmap, geant, group, logical)) {
    cout << "Failed to set mapping table entry!" << endl;
    exit(0);
  } //if
 
  beamPipe->GetColorTable()->AddPatternMatch       ("ZDCBeamPipe", kGray);
  beamPipe->GetTransparencyTable()->AddPatternMatch("ZDCBeamPipe", 0);
  
  

  // A unified user call which places assembled detector volume in a proper place in MASTER (top)
  // coordinate system, puts this MASTER (top) volume into GEANT volume tree, and dumps this tree 
  // together with EicRoot mapping table in one file;
  beamPipe->FinalizeOutput();

  // Yes, always exit;
  exit(0);
} // zdc()

