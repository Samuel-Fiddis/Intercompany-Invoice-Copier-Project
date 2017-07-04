#include "../../std_lib_facilities.h"
#include <direct.h>
#include <fstream>

	// Created by Samuel Fiddis
	// Last updated 12.02.16

	// This program is designed to parse a text file to form a list of invoice numbers labeled in the form A#####
	// (where A is a specific letter value and # is any integer value).
	// It will then search for pdf files which are labled with these invoice numbers and move them from directories
	// (which are dependent on the invoices starting letter) to another directory.
	// Any invoices which cannot be found will be listed in two separate text files depending on whether the invoices were approved or unapproved.

class directories {
public:
	directories(char c, string ad, string und);

	char get_letter() const { return prefix; }
	string get_approved_dir() const { return approved_dir; }
	string get_unapproved_dir() const { return unapproved_dir; }

private:
	char prefix;
	string approved_dir;
	string unapproved_dir;
};

directories::directories(char c, string ad, string und) :
	approved_dir{ ad }, unapproved_dir{ und }
{
	if (isalpha(c)) prefix = c;
}

//CopyFile is a simple function that copies a file from arg1 to arg2
int CopyFile(string initialFilePath, string outputFilePath)
{
	ifstream initialFile(initialFilePath.c_str(), ios::in | ios::binary);
	ofstream outputFile(outputFilePath.c_str(), ios::out | ios::binary);

	//defines the size of the buffer	
	initialFile.seekg(0, ios::end);
	long fileSize = initialFile.tellg();

	//Requests the buffer of the predefined size as long as both the input and output files are open
	if (initialFile.is_open() && outputFile.is_open())
	{
		short * buffer = new short[fileSize / 2 + 1];		//Determine the file's size				
		initialFile.seekg(0, ios::beg);					//Then starts from the beginning

		initialFile.read((char*)buffer, fileSize);		//Then read enough of the file to fill the buffer	
		outputFile.write((char*)buffer, fileSize);		//And then write out all that was read	
		delete[] buffer;
	}

	//If there were any problems with the copying process, let the user know	
	else if (!outputFile.is_open())
	{
		cout << "I couldn't open " << outputFilePath << " for copying!\n";
		return 0;
	}

	else if (!initialFile.is_open())
	{
		cout << "I couldn't open " << initialFilePath << " for copying!\n";
		return 0;
	}

	initialFile.close();
	outputFile.close();
	return 1;
}

// Test if a string is an invoice number
bool test_if_invoice(string t)
{
	return t.size() == 6 && (t[0] == 'Q' || t[0] == 'R' || t[0] == 'N' || t[0] == 'L' || t[0] == 'M')
		&& isdigit(t[1]) && isdigit(t[2]) && isdigit(t[3]) && isdigit(t[4]) && isdigit(t[5]);
}

// Place the invoice values into a vector
void find_invoice_numbers(string file_name, vector<string>& invoice_numbers)
{
	ifstream invoices{ file_name };

	for (string temp; invoices >> temp;)
		if (test_if_invoice(temp)) invoice_numbers.push_back(temp);

	//This part deletes duplicats in the vector
	sort(invoice_numbers);

	for (int i = 1; i < invoice_numbers.size(); ++i)
		if (invoice_numbers[i - 1] == invoice_numbers[i])
		{
			invoice_numbers.erase(invoice_numbers.begin() + i);
			i--;
		}
}

// Determine the job number that the invoice is for
// Currently very ridgid
string find_job_number(string file_name)
{
	ifstream is{ file_name };

	string job = "SJ";
	string temp;
	while (is >> temp)
	{
		if (temp == "CLIENT") {
			is >> temp;
			job.append(temp);
			break;
		}
	}
	while (is >> temp) {
		if (temp == "PRODUCT") {
			is >> temp;
			job.append(temp);
			break;
		}
	}
	while (is >> temp) {
		if (temp == "JOB") {
			is >> temp;
			job.append(temp);
			break;
		}
	}
	return job;
}

// Program to find and move approved invoice pdf files from one directory to another
void move_approved_invoices(const vector<string>& invoices, const vector<directories>& dir, const string& Suffix, string missing_output)
{
	ofstream missing{ missing_output };

	for (int i = 0; i < invoices.size(); ++i)
	{
		string source = "";
		int result = 0;

		for (int j = 0; j < dir.size(); ++j)
			if (invoices[i][0] == dir[j].get_letter()) source = dir[j].get_approved_dir();

		source.append(invoices[i] + Suffix);
		result = CopyFile(source, invoices[i] + Suffix);
		if (result == 1)
			puts("File successfully copied.");
		else
		{
			remove((invoices[i] + Suffix).c_str());		//Remove missing files that have been copied to fix bug
			missing << invoices[i] << " \n";
		}
	}
}

// Program to find and move unapproved invoice pdf files from one directory to another
void move_unapproved_invoices(const vector<string>& invoices,const vector<directories>& dir)
{
	ofstream missing{ "Missing_Unapproved_Invoices.txt" };
	string Unapproved_Suffix = ".pdf";

	for (int i = 0; i < invoices.size(); ++i)
	{
		string source = "";
		int result = 0;

		for (int j = 0; j < dir.size(); ++j)
			if (invoices[i][0] == dir[j].get_letter()) source = dir[j].get_unapproved_dir();
		
		source.append(invoices[i] + Unapproved_Suffix);
		result = CopyFile(source, invoices[i] + Unapproved_Suffix);
		if (result == 1)
			puts("File successfully copied.");
		else
		{
			remove((invoices[i] + Unapproved_Suffix).c_str());		//Remove missing files that have been copied to fix bug
			missing << invoices[i] << '\n';
		}
	}
}

//------------------------------------------------------------------------------------------------------------

int main()
{
	// This is a vector containing a list of all of the directories which 
	// files beginning with their designated prefix can be found in.
	vector<directories> Directory_List;

	directories N_dir{ 'N',"Y:\\Control\\Invoice Scans\\2014\\Authorised Scans\\" ,"Y:\\Control\\Invoice Scans\\2014\\MOST RECENT SCANS\\" };
	directories L_dir{ 'L',"","Y:\\Control\\Invoice Scans\\2012\\" };
	directories M_dir{ 'M',"Y:\\Control\\Invoice Scans\\2013\\Authorised Invoices\\","" };
	directories R_dir{ 'R',"Y:\\Control\\Invoice Scans\\2016\\Authorised Scans 2016\\","Y:\\Control\\Invoice Scans\\2016\\Most Recen Scans 2016\\" };
	directories Q_dir{ 'Q',"Y:\\Control\\Invoice Scans\\2015\\Authorised Scans 2015\\","Y:\\Control\\Invoice Scans\\2015\\Most Recent Scans 2015\\" };
	Directory_List.push_back(N_dir);
	Directory_List.push_back(L_dir);
	Directory_List.push_back(M_dir);
	Directory_List.push_back(R_dir);
	Directory_List.push_back(Q_dir);

	vector<string> Invoice_Numbers;				// Stores Invoice Numbers
	vector<string> Missing_Invoice_Numbers;
	string file_name = "Invoice_Finder.txt";	// Txt file to find invoices from
	
	find_invoice_numbers(file_name, Invoice_Numbers);

	for (int i = 0; i < Invoice_Numbers.size(); ++i)
		cout << Invoice_Numbers[i] << '\n';

	move_approved_invoices(Invoice_Numbers,Directory_List, ".S.pdf", "Missing_Approved_Invoices.txt");

	string missing_invoices = "Missing_Approved_Invoices.txt";
	find_invoice_numbers(missing_invoices, Missing_Invoice_Numbers);

	for (int i = 0; i < Missing_Invoice_Numbers.size(); ++i)
		cout << Missing_Invoice_Numbers[i] << '\n';

	move_unapproved_invoices(Missing_Invoice_Numbers,Directory_List);
}
