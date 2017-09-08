#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <string> 
#include <list>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <vector>
#include <sys/types.h>
#include  <io.h>  
#include <http_client.h>


using namespace std;


void read_from_input_stock(list<unsigned short int> *stock_list) {

	fstream fp;

	unsigned short int t;

	fp.open("../input_stock.txt", ios::in);
	if (!fp) {
		cout << "Fail to open file: input_stock.txt" << endl;
		return;
	}

	while (fp >> t)
	{
		stock_list->push_back(t);
	}

	fp.close();

}
void reverse_avg(int day, float detx, float dety, list<float> *L) {

	float coeff = -1;
	float sum = 0;
	int i = 0;

	for (; i< 1 / detx && i < day; i++) {
		coeff = coeff + detx;
		L->push_back(coeff);
	}

	for (; i<day; i++) {

		coeff = coeff + dety;
		L->push_back(coeff);

	}

	for (std::list<float>::iterator it = L->begin(); it != L->end(); ++it) {
		sum += *it;
	}
	L->push_back(sum);
	/*
	for (std::list<float>::iterator it=L->begin(); it != L->end(); ++it){
	cout<<*it<<endl;
	}

	cout<<"*******\n";
	*/
}
void download_date(unsigned short int stock, string *date) {

	int read_times = 0;

	fstream fp, fp2;

	fp.open("../tclrunning_log.txt", std::fstream::out | ios::app);

	fp << "http://tw.stock.yahoo.com/q/q?s=$stock" << endl;

	fp.close();

	string cmd = "wget --no-check-certificate http://tw.stock.yahoo.com/q/q?s=" + to_string(stock) + " -O date.html";
	;
	system(cmd.c_str());

	fp2.open("date.html", ios::in);

	string  str;
	while (getline(fp2, str))
	{
		std::size_t pos = str.find("<td width=160 align=right><font color=#3333FF class=tt>");      // position of "live" in str

		if ((int)pos > 0) {

			std::string str3 = str.substr(pos + 67);
			std::string str4 = str3.substr(0, 9);
			*date = str4;

			break;

		}
	}

	fp2.close();
}
void csv_split(string s, vector<string> *V) {

	istringstream iss(s);
	string token;
	while (getline(iss, token, ','))
	{
		V->push_back(token);
	}

}
string date = "105/10/26";
int auto_renew = 1;
int update_finance = 0;
int dont_use_yahoo = 0;
int num_major_day = 3;



int main() {

	float det1 = 0.5;
	float det2 = 0.0092;
	float det3 = 0.5;
	float det4 = 0.00845;
	int day = 20;


	list<float> up_list;
	list<float> dn_list;
	list<unsigned short int>stock_list;

	reverse_avg(day, det1, det2, &up_list);
	reverse_avg(day, det3, det4, &dn_list);
	read_from_input_stock(&stock_list);

	download_date(3008, &date);

	int stock_left = stock_list.size() + 1;


	for (std::list<unsigned short int>::iterator it = stock_list.begin(); it != stock_list.end(); ++it) {

		bool exist_flag = false;

		stock_left--;

		if ((_access((to_string(*it) + ".csv").c_str(), 0)) != -1)
		{


			fstream fp;
			string str;

			exist_flag = true;

			fp.open((to_string(*it) + ".csv").c_str(), ios::in);

			getline(fp, str);
			getline(fp, str);
			getline(fp, str);
			fp.close();
			std::size_t pos = str.find(",");
			std::string str2 = str.substr(0, pos);

			if (str2 == date) {
				continue;
			}
		}

		if (date == "") {
			date = "101/01/01";
		}

		{
			fstream fp;
			fp.open("../tclrunning_log.txt", std::fstream::out | ios::app);
			fp << *it << "\tNum of left stocks for close_price.tcl = " << stock_left << endl;
			if (stock_left % 10 == 9) {

				//puts $flog [clock format [clock seconds] -format "%H:%M:%S"]
			}

			fp.close();
		}

		string eps0 = "";
		string eps1 = "";
		string yymm = "";
		string capt = "";
		string MoM = "";
		string YoY = "";
		if (update_finance == 0 && exist_flag == true) {

			fstream fp;
			string str;


			fp.open((to_string(*it) + ".csv").c_str(), ios::in);

			getline(fp, str);

			{
				vector<string> splits;
				csv_split(str, &splits);
				eps0 = splits[17] + "," + splits[18] + "," + splits[19];

			}

			getline(fp, str);

			{

				vector<string> splits;
				csv_split(str, &splits);
				eps1 = splits[17] + "," + splits[18] + "," + splits[19];

				capt = splits[16];
				yymm = splits[20];
				MoM = splits[21];
				YoY = splits[22];
			}



			fp.close();

		}

		if (eps0 == "" || eps1 == "") {

			/*
			set EPS_list [get_finance $stock]
			set eps0 [lindex $EPS_list 0]
			set eps1 [lindex $EPS_list 1]
			if {[llength [csv_split $eps0]]<3} {
			set eps0 "NA,NA,NA"
			set eps1 "0,0,0"
			}
			*/
		}

		/*  if {$update_finance==0 && [file exists $stock_file]} {
		set fr [open $stock_file "r"]
		gets $fr s
		set splits [csv_split $s]
		set eps0 "[lindex $splits 17],[lindex $splits 18],[lindex $splits 19]"
		gets $fr s
		set splits [csv_split $s]
		set eps1 "[lindex $splits 17],[lindex $splits 18],[lindex $splits 19]"
		set capt [lindex $splits 16]
		set yymm [lindex $splits 20]
		set MoM [lindex $splits 21]
		set YoY [lindex $splits 22]
		close $fr
		*/
	}




	return 0;
}