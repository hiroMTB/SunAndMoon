#pragma once

void exportTrj(){
    
    string message = "Export .ply file, please select file prefix name";
    string defaultFilename = "sm_";
    ofFileDialogResult saveFileResult = ofSystemSaveDialog(defaultFilename, message);
    if (saveFileResult.bSuccess){
        
        string filename = saveFileResult.filePath;
        
        if(sun.bDraw){
            sun.trj.save(filename + "_sun-sky.ply");
            if(room.bDraw)
                room.sunWallPath.save(filename + "_sun-wall.ply");
        }
        
        if(moon.bDraw){
            moon.trj.save(filename + "_moon-sky.ply");

            if(room.bDraw)
                room.moonWallPath.save(filename + "_moon-wall.ply");
        }
        
    }else{
        cout << "error" << endl;
    }
}

// export
ofParameter<void> btnExport{"Start Export"};
ofParameterGroup expGrp{"Export", btnExport};
