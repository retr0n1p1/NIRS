#include<iostream>
using namespace std;

#define pltCh 1 // сколько графиков строить в функции плот
#define N 10 // число устреднененных значений с каждой стороны от текущей ячейки при фильтрации
#define histsrc 1 // канал по которому будет строится гистограмма

//ROOT::EnableImplicitMT(6); // Using all cpu cores
//gROOT->SetBatch(kTRUE);

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

void spectre(const array<vector<int>, 8> &der, vector<int> &buffer, const int &nint, int ch) {
int peak = *max_element(der[ch-1].begin(), der[ch-1].end());
if(peak == 0) return;
int count_peaks = 0;
for(auto& i : der[ch-1]) {
	if(i==peak) {
		count_peaks++;
		buffer.push_back(i);
	}
if(count_peaks > 1) {
	buffer.pop_back();
	return;
}
}
}
  

void trim(array<vector<int>, 8> &der, const int &nint){
for(int i = 0; i<8; i++){
	int mean = 0;
	for(int j = 0; j<nint; j++) mean = der[i][j] + mean;
	mean = round((float)mean / nint);

	for(int j = 0; j<nint; j++){
		der[i][j] = der[i][j] - mean;
		if(der[i][j] < 0) der[i][j] = 0;
	}
    
	int max1 = 0, max2 = 0;
	for(auto j : der[i]){
		if(j>=max1) max1=j;
		else if(j<max1 and j>=max2) max2=j;
	}

	int count_peaks = 0;

	for(int j = 0; j<nint; j++){
		if(der[i][j] < 0) der[i][j] = 0;
		if(der[i][j]==max1 or der[i][j]==max2) count_peaks++;
	}
    
	if(count_peaks>2){
		for(auto& j : der[i]) j=0;
    	}
}
}


void filter(array<vector<int>, 8> &der,const array<vector<int>*, 8> &ch, const int &nint){
for(int k = 0; k<8; k++){
	for(int j = 0; j<N; j++) der[k].push_back((*ch[k])[j]);
	for(int j = N; j<nint-N; j++){
		int summ = 0;
		for(int f = -N; f<N+1; f++){
			if(f==0) continue;
			summ = summ+(*ch[k])[j+f];
		}
		summ = (summ/N)/2;
		der[k].push_back(-summ);
	}
	for(int j = -N; j<0; j++) der[k].push_back((*ch[k])[nint+j]);
}
}


void plot(const array<vector<int>, 8> &ch, const vector<int> &X, int &entry, int &nint){

TCanvas *c = new TCanvas("c", "8 Graphs", 1920, 1080); 
c->Divide(1, pltCh);

for (int i = 1; i < pltCh+1; i++) {
	c->cd(i);
	TGraph *g = new TGraph(nint, X.data(), ch[i-1].data());
	g->Draw("AL"); 
}
string name = "png/"+to_string(entry)+".png";
c->SaveAs(name.c_str());
delete c;
}


void analyze(const char *fname = "sample.dat"){
string filename = fname;

if (filename.length() >= 5)
	filename.erase(filename.length() - 5);

TFile* f = TFile::Open(fname);
if(!f || f->IsZombie()){
	cerr<<"Failed to open file" << endl;
	return;
}
TTree* t = nullptr;
f->GetObject("DATA", t);
if(!t){
	cerr<<"Failed to open tree"<< endl;
	return;
}

int timescale, nint, entry;
t->SetBranchAddress("nint", &nint);
t->SetBranchAddress("timescale", &timescale);
t->GetEntry(0);
entry = t->GetEntries();

vector<int> X;
for(int i = 0; i<nint; i++)  X.push_back(timescale*i); 

array<vector<int>*, 8> ch;
for(int i = 0; i<8; i++) ch[i] = nullptr;

t->SetBranchAddress("Channel1", &ch[0]);
t->SetBranchAddress("Channel2", &ch[1]);
t->SetBranchAddress("Channel3", &ch[2]);
t->SetBranchAddress("Channel4", &ch[3]);
t->SetBranchAddress("Channel5", &ch[4]);
t->SetBranchAddress("Channel6", &ch[5]);
t->SetBranchAddress("Channel7", &ch[6]);
t->SetBranchAddress("Channel8", &ch[7]);

vector<int> buffer;

for(int i = 0; i<entry; i++){
	t->GetEntry(i);
	show_progress(i, entry);
	array<vector<int>, 8> der;
	filter(der, ch, nint);
	trim(der, nint);
	spectre(der, buffer, nint, histsrc);
	//plot(der, X, i, nint);
}

TCanvas *c = new TCanvas("c", "8 Graphs", 1920, 1080);  

int maxi = 1+*max_element(buffer.begin(), buffer.end());
int mini = -1+*min_element(buffer.begin(), buffer.end());

TH1F* h = new TH1F("hist_100_bin", "", 100, mini, maxi);

for(int i = 0; i<buffer.size(); i++) h->Fill(buffer[i]);
h->Write();
h->Draw();
//TH1F* h_re = (TH1F*) h->Rebin(5, "h_rebinned");  // уменьшает число биннов в 2 раза
//h_re->Draw();	

string name = filename+"_hist_ch"+to_string(histsrc)+".png";
c->SaveAs(name.c_str());
delete c;

f->Close();
}
