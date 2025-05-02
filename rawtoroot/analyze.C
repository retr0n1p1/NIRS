#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>


using namespace std;

ROOT::EnableImplicitMT(); // Using all cpu cores
gROOT->SetBatch(kTRUE);

void show_progress(int current, int total) { //progress-bar
	int width = 50; 
    	float progress = (float)current / total;
    	int pos = width * progress;

    	cout << "\r";
    	for (int i = 0; i < width; ++i) {
        	if (i < pos) cout << "#";
        	else cout << " ";
    	}
    	cout << int(progress * 100.0) << "%";
    	cout.flush();
}

inline bool looksLikeDate(const string& s) 
{
	if (s.size() < 10) return false;
    	return  isdigit(s[0]) && isdigit(s[1]) &&
        	isdigit(s[2]) && isdigit(s[3]) &&
         	s[4] == '-' &&
           	isdigit(s[5]) && isdigit(s[6]) &&
           	s[7] == '-' &&
           	isdigit(s[8]) && isdigit(s[9]);
}

void analyze(const char *fname = "sample.dat", int entries = 0, bool quiet = false)
{	
	size_t lines = 0;
   	
	if(entries==0 && !quiet){
		cout<<"Calculating data file length for progress indication..."<<endl;
		ifstream file(fname, ios::in | ios::binary);
		const size_t buffer_size = 1 << 20; // 1 МБ
		char buffer[buffer_size];

		while (file.read(buffer, buffer_size) || file.gcount()) 
        		for (std::streamsize i = 0; i < file.gcount(); ++i) 
            			if (buffer[i] == '\n') ++lines;
        
		file.close();
		cout<<"Start processing data file..."<<endl;
   	}


	string filename = fname;

        if (filename.length() >= 4) 
    	    filename.erase(filename.length() - 4);
	filename = filename + ".root";

	TFile* f = new TFile(filename.c_str(), "RECREATE");
	TTree* t = new TTree("DATA", "DATA");
	size_t LineCounter = 0;

	string parseD = "";
	string parseT = "";
	TDatime DT;
	Int_t TS;
	Int_t nint;
	Int_t nfirst;
	vector<int> bufer1;
	vector<int> bufer2;
	vector<int> bufer3;
	vector<int> bufer4;
	vector<int> bufer5;
	vector<int> bufer6;
	vector<int> bufer7;
	vector<int> bufer8;
        vector<int> row;
        row.reserve(40000);   

	t->Branch("TimeUt", &DT);
	t->Branch("timescale", &TS);
	t->Branch("nint", &nint);
	t->Branch("nfirst", &nfirst);
	t->Branch("Channel1", &bufer1);
	t->Branch("Channel2", &bufer2);
	t->Branch("Channel3", &bufer3);
	t->Branch("Channel4", &bufer4);
	t->Branch("Channel5", &bufer5);
	t->Branch("Channel6", &bufer6);
	t->Branch("Channel7", &bufer7);
	t->Branch("Channel8", &bufer8);


	ifstream fin(fname);
    	if (!fin.is_open()) {
        	cerr << "Cannot open file " << fname << '\n';
        return;
    }

    string line;
    while (getline(fin, line)) {
        if (line.empty() || !looksLikeDate(line))
        	continue;                                  
	
	for(size_t i = 0; i<4; i++) parseD = parseD+line[i];
	for(size_t i = 5; i<7; i++) parseD = parseD+line[i];
	for(size_t i = 8; i<10; i++) parseD = parseD+line[i];
	for(size_t i = 11; i<13; i++) parseT = parseT+line[i];
	for(size_t i = 14; i<16; i++) parseT = parseT+line[i];
	for(size_t i = 17; i<19; i++) parseT = parseT+line[i];
	
	string subb = line.substr(19, 39);
	istringstream(subb)>>TS>>nint>>nfirst;

	int date = stoi(parseD);
	int time = stoi(parseT);
	DT.Set(date, time);
	
	parseD.clear();
	parseT.clear();

        size_t br = line.find('{');
        if (br == string::npos) continue;         

        const char* p = line.data() + br + 1;          
        const char* end = line.data() + line.size();
	
	row.clear();

        while (p < end && *p != '}') {
        	while (p < end && isspace(*p)) ++p;
		
		char* next;
            	long val = strtol(p, &next, 10);
            	if (p == next) {                          
                	++p;                                  
                	continue;
            	}
            	row.push_back(static_cast<int>(val));
            	p = next;

            	while (p < end && (*p == ',' || isspace(*p)))
                	++p;
        }
		
	bufer1.clear();
	bufer2.clear();
	bufer3.clear();
	bufer4.clear();
	bufer5.clear();
	bufer6.clear();
	bufer7.clear();
	bufer8.clear();

     	if(row.size() != 0){
		int ch = row.size() / 8;
		for(int i = 0; i<ch; i++) bufer1.push_back(row[i]);
		for(int i = ch; i<2*ch; i++) bufer2.push_back(row[i]);
		for(int i = 2*ch; i<3*ch; i++) bufer3.push_back(row[i]);
		for(int i = 3*ch; i<4*ch; i++) bufer4.push_back(row[i]);
		for(int i = 4*ch; i<5*ch; i++) bufer5.push_back(row[i]);
		for(int i = 5*ch; i<6*ch; i++) bufer6.push_back(row[i]);
		for(int i = 6*ch; i<7*ch; i++) bufer7.push_back(row[i]);
		for(int i = 7*ch; i<row.size(); i++) bufer8.push_back(row[i]);
        	LineCounter++;
		t->Fill();
	}
	if(entries!=0){
		if(!quiet){
		       	show_progress(LineCounter, entries);
			this_thread::sleep_for(chrono::milliseconds(50));
		}
		if(LineCounter == entries) break;
	}
	else if(!quiet){
	       	show_progress(LineCounter, lines);
		this_thread::sleep_for(chrono::milliseconds(50));
	     }

	}
	t->Write();
	f->Close();
	cout<<endl;
}
