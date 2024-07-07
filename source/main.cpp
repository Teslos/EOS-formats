#include "main.h"
#include <fstream>
#include <string>

void verify_and_save_parameter(string key, string value, float *parameter_to_save) {
    // convert to double or float number
    *parameter_to_save = stod(value);
}

void verify_and_save_parameter_string(string key, string value, string *parameter_to_save) {
    // we pass string so it doesnt need conversion
    *parameter_to_save = value;
}

void load_parameters_from_file(string filename, struct parameters *my_parameters)
{
    std::ifstream input_file;
    input_file.open(filename);
    string line;
    string key, value;

    while(std::getline(input_file,line)) {
        // read the next line in the file
        // skip the blank lines
        if (line.empty()) {
            continue;
        }

        int position_in_string; 
        if (line.rfind("//",0) == 0) {
            // ignore the comment lines starting with token (//)
            continue;
        } else if ((position_in_string = line.rfind("//")) > 0) {
            line = line.substr(0,position_in_string); 
        }

        // split the line into a key and value
        int equal_token_pos = line.find("=");

        // parse key and value from the line
        key = line.substr(0,equal_token_pos);
        value = line.substr(equal_token_pos+1,line.length() - 1);

        // check if the key is a known parameter. If is a known
        // parameter, check validity, and store it
        if ("power" == key) {
             verify_and_save_parameter(key, value, &(my_parameters->power));
        } else if ("velocity" == key) {
             verify_and_save_parameter(key, value, &(my_parameters->velocity));
        } else if ("recoatingtime" == key) {
             verify_and_save_parameter(key, value, &(my_parameters->recoatingtime));
        } else if ("slicer_input_filename" == key) {
             verify_and_save_parameter_string(key, value, &(my_parameters->slicer_input_filename));
        } else if ("abaqus_output_filename" == key) {
             verify_and_save_parameter_string(key, value, &(my_parameters->abaqus_output_filename));
        } else {
             cout << "load parameters: Parameter is not recognized: " << key << endl;
        }

    } 
}

void initialize_parameters(struct parameters &params) {
    params.layerPos = 0;
    params.power = 0.0;
    params.recoatingtime = 0.0;
    params.time = 0.0;
    params.velocity = 0.0;
}

struct parameters load_parameters(string output_dir, string filename)
{
    struct parameters my_parameters;
    initialize_parameters(my_parameters);
    std::ifstream f(filename);
    bool file_exits = f.good(); 
    if (file_exits) {
        load_parameters_from_file(filename, &my_parameters);
    } else {
        cout << "File doesn't exist: " << filename << endl;
    }
    return my_parameters;
}


int main(int argc, char* argv [])
{
	
	//clJobSliceFile sliFile;
	clSliFile sliFile;
	clSliceData sliceData;
    string parameters_input_file;
    struct parameters my_parameters;

	if (argc > 1)
	{
		parameters_input_file = argv[1];
	}
	else
	{
        cout << "Please give input file: main.exe input_file.inp" << endl;
	}
	
    // load parameters
    my_parameters = load_parameters("", parameters_input_file);
    string slicer = my_parameters.slicer_input_filename;
    string abaqus = my_parameters.abaqus_output_filename;
    cout << my_parameters.power << endl;
    cout << my_parameters.velocity << endl;
    cout << my_parameters.recoatingtime << endl;
    cout << my_parameters.slicer_input_filename << endl;
    cout << my_parameters.abaqus_output_filename << endl;

    sliFile.readFromFile(slicer.c_str());

    // output file
	std::ofstream outfile;
	outfile.open(abaqus);

    // define all SLM parameters	
	float power = my_parameters.power;   // this is mW
	float velocity = my_parameters.velocity;   // this is in mm/s
	float time = 0.0;
	float recoatingtime = my_parameters.recoatingtime; // this is in seconds
	int layerIndex = 0;
	int partIndex = 0;
	float layerPos = 0.0;
	float layerThickness = sliFile.getLayerThickness();

	// get all the layers
	for (layerIndex = 0; layerIndex < sliFile.getLayerCount(partIndex); layerIndex++) {
		sliFile.readSliceData(&sliceData, partIndex, layerIndex);
		layerPos = layerIndex * layerThickness;
		clSliceData::tyMatrix TransMatrix;
		clSliceData::IdentityMatrix(&TransMatrix);

		
		int partCount = sliceData.getPartCount();
		
		for (int part = 0; part < partCount; part++)
		{
			int objectCount = sliceData.getObjectCount(part);
			for (int object = 0; object < objectCount; object++)
			{
				float * points = sliceData.getObjectPointsTransformed(part, object, TransMatrix);
				float x1, y1, x2, y2;
				x1 = x2 = *points++;
				y1 = y2 = *points++;
				int count = sliceData.getPointCount(part, object);
				outfile << time << "," << x1 << "," << y1 << "," << layerPos << "," << power << endl;		
				// @ i dont understand why the last point is returned with unrealistic values.
				// @ should investigate more.
				for (int i = count; i > 1; i--) {
					x2 = *points++;
					y2 = *points++;
					// calculate time and add to build time
					float distance = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
					time += distance / velocity;
					// hack: it turn off every second line (scanning of the laser)
					if (i % 2)
						outfile << time << "," << x2 << "," << y2 << "," << layerPos << "," << power << endl;
					else
						outfile << time << "," << x2 << "," << y2 << "," << layerPos << "," << 0 << endl;
					x1 = x2;
					y1 = y2;
				}


				//sliceData.drawRasteredObject(&imgFilled, &imgPolyLine, part, object, TransMatrix, object, w, h);
			}
		}

		// @too be changed: for every layer we need some time to recoat
		time += recoatingtime;
	}
	outfile.close();
	system("pause");
}





