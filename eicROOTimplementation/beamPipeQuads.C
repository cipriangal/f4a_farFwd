
void beamPipeQuads()
{
  // Load basic libraries;
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  EicGeoParData *beamPipe = new EicGeoParData("BEAMPIPEQUADS", 0, 0);
  beamPipe->SetFileName("beampipeQUADS.root");

  Double_t length[5];
  Double_t inRadius[5];
  Double_t outRadius[5];
  Double_t rotationAngle[5];
  Double_t xCenter[5];
  Double_t yCenter[5];
  Double_t zCenter[5];
    
  TGeoRotation * rw[5];
    
  //B0apf
  Double_t length[0]            = 1600; //17500.0;//16500; //23212.8;
  Double_t inRadius[0]          = 40.0; //168.0; //inner radius
  Double_t outRadius[0]         = 41.0; //170.0; //outer radius
  Double_t rotationAngle[0]     = 25.0; //in mrad
  Double_t xCenter[0]           = 198.0;
  Double_t yCenter[0]           = 0.0;
  Double_t zCenter[0]           = 7700.0; //30315.0;
  
  //Q1apf	
  Double_t length[1]            = 1500; //17500.0;//16500; //23212.8;
  Double_t inRadius[1]          = 51.0; //168.0; //inner radius
  Double_t outRadius[1]         = 52.0; //170.0; //outer radius
  Double_t rotationAngle[1]     = 19.5;
  Double_t xCenter[1]           = 244.7;
  Double_t yCenter[1]           = 0.0;
  Double_t zCenter[1]           = 9228.0; //30315.0;

  //Q1bpf
  Double_t length[2]            = 2200; //17500.0;//16500; //23212.8;
  Double_t inRadius[2]          = 70.0; //168.0; //inner radius
  Double_t outRadius[2]         = 72.0; //170.0; //outer radius
  Double_t rotationAngle[2]     = 15.0;
  Double_t xCenter[2]           = 300.5;
  Double_t yCenter[2]           = 0.0;
  Double_t zCenter[2]           = 11063.0; //30315.0;

  //Q2pf
  Double_t length[3]            = 4400; //17500.0;//16500; //23212.8;
  Double_t inRadius[3]          = 120.0; //168.0; //inner radius
  Double_t outRadius[3]         = 122.0; //170.0; //outer radius
  Double_t rotationAngle[3]     = 100.0;
  Double_t xCenter[3]           = 395.0;
  Double_t yCenter[3]           = 0.0;
  Double_t zCenter[3]           = 14167.0; //30315.0;

  //B1pf
  Double_t length[4]            = 3300; //17500.0;//16500; //23212.8;
  Double_t inRadius[4]          = 122.0; //168.0; //inner radius
  Double_t outRadius[4]         = 124.0; //170.0; //outer radius
  Double_t rotationAngle[4]     = 100.0;
  Double_t xCenter[4]           = 480.0;
  Double_t yCenter[4]           = 0.0;
  Double_t zCenter[4]           = 18067.0; //30315.0;

  rw[0] = new TGeoRotation();
  double angle1 = .025*TMath::RadToDeg();
  
  rw[1] = new TGeoRotation();
  double angle2 = .0195*TMath::RadToDeg();

  rw[2] = new TGeoRotation();
  double angle3 = .015*TMath::RadToDeg();

  rw[3] = new TGeoRotation();
  double angle4 = .015*TMath::RadToDeg();

  rw[4] = new TGeoRotation();
  double angle5 = .036*TMath::RadToDeg();

  rw[0]->RotateY(angle1);
  rw[1]->RotateY(angle2);
  rw[2]->RotateY(angle3);
  rw[3]->RotateY(angle4);
  rw[4]->RotateY(angle5);


  for(int i = 0; i < 5; i++){

    TGeoTube *pipe = new TGeoTube(Form("QuadBeamTube%d",i), 0.1*inRadius[i], 0.1 * outRadius[i], 0.1 * length[i]/2);
    TGeoVolume *vpipe = new TGeoVolume(Form("QuadBeamPipe%d",i), pipe, beamPipe->GetMedium("aluminum"));
        
    EicGeoMap *fgmap = beamPipe->CreateNewMap();
    fgmap->AddGeantVolumeLevel(Form("QuadBeamPipe%d",i),5);
    fgmap->SetSingleSensorContainerVolume(Form("QuadBeamPipe%d",i));

    beamPipe->AddLogicalVolumeGroup(0, 0, 5);//waferNum);


    UInt_t geant[1] = {0}, group = i, logical[3] = {0, 0, 0};
    
    if (beamPipe->SetMappingTableEntry(fgmap, geant, group, logical)) {
      cout << "Failed to set mapping table entry!" << endl;
      exit(0);
    } //if
        
    beamPipe->GetTopVolume()->AddNode(vpipe, 0, new TGeoCombiTrans(0.1 * xCenter[i] ,0.1 *  yCenter[i], 0.1 * zCenter[i], rw[i]));
  }

  beamPipe->GetColorTable()->AddPatternMatch       ("Quad", kGray);
  beamPipe->GetTransparencyTable()->AddPatternMatch("Quad", 0);
  
  
  // A unified user call which places assembled detector volume in a proper place in MASTER (top)
  // coordinate system, puts this MASTER (top) volume into GEANT volume tree, and dumps this tree 
  // together with EicRoot mapping table in one file;
  beamPipe->FinalizeOutput();

  // Yes, always exit;
  exit(0);
} // zdc()

