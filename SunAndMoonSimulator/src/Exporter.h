#pragma once

void exportTrj(){
    
    string message = "Export .ply file, please select file prefix name";
    string defaultFilename = "sm_";
    ofFileDialogResult saveFileResult = ofSystemSaveDialog(defaultFilename, message);
    if (saveFileResult.bSuccess){
        
        string filename = saveFileResult.filePath;
        
        if(bExportSunSky){
            sun.trj.save(filename + "_sun-sky.ply");
        }
        
        if(bExportMoonSky){
            moon.trj.save(filename + "_moon-sky.ply");
        }
        
        if(bExportSunWall){
            room.sunWallPath.save(filename + "_sun-wall.ply");
        }
        
        if(bExportMoonWall){
            room.moonWallPath.save(filename + "_moon-wall.ply");
        }
        
    }else{
        cout << "error" << endl;
    }
}

// export
ofParameter<bool> bExportSunSky{"export sun : sky trajectry", true};
ofParameter<bool> bExportMoonSky{"export moon : sky trajectry", true};
ofParameter<bool> bExportSunWall{"export sun : wall trajectry", true};
ofParameter<bool> bExportMoonWall{"export moon : wall trajectry", true};
ofParameter<void> btnExport{"Start Export"};
ofParameterGroup expGrp{"Export", bExportSunSky, bExportMoonSky, bExportSunWall, bExportMoonWall, btnExport};
