#include "../../std_lib_facilities.h"
#include <direct.h>
#include <fstream>

class email_list {
	
public:
	string job;
	vector<string> emails;
	vector<string> first_names;
	vector<string> last_names;
};



//CopyFile is a simple function that copies a file from arg1 to arg2
int CopyFile(string initialFilePath, string outputFilePath)
{		
	ifstream initialFile(initialFilePath.c_str(), ios::in|ios::binary);	
	ofstream outputFile(outputFilePath.c_str(), ios::out|ios::binary);	
	
	//defines the size of the buffer	
	initialFile.seekg(0, ios::end);	
	long fileSize = initialFile.tellg();	
	
	//Requests the buffer of the predefined size as long as both the input and output files are open
	if(initialFile.is_open() && outputFile.is_open())	
	{
		short * buffer = new short[fileSize/2 + 1];		//Determine the file's size				
		initialFile.seekg(0, ios::beg);					//Then starts from the beginning
	
		initialFile.read((char*)buffer, fileSize);		//Then read enough of the file to fill the buffer	
		outputFile.write((char*)buffer, fileSize);		//And then write out all that was read	
		delete[] buffer;
	}	

	//If there were any problems with the copying process, let the user know	
	else if(!outputFile.is_open())	
	{		
		cout<<"I couldn't open "<<outputFilePath<<" for copying!\n";		
		return 0;	
	}	

	else if(!initialFile.is_open())	
	{		
		cout<<"I couldn't open "<<initialFilePath<<" for copying!\n";		
		return 0;	
	}

	initialFile.close();	
	outputFile.close();	
	return 1;
}

// Test if a string is an invoice number
bool test_if_invoice(string t)
{
	return t.size() == 6 && (t[0] == 'Q' || t[0] == 'R' || t[0] == 'N') 
						&& isdigit(t[1]) && isdigit(t[2]) && isdigit(t[3])
						&& isdigit(t[4]) && isdigit(t[5]);
}

// Place the invoice values into a vector
void find_invoice_numbers(string file_name, vector<string>& invoice_numbers)
{
	ifstream invoices{ file_name };

	for (string temp; invoices >> temp;)
	{
		if (test_if_invoice(temp)) invoice_numbers.push_back(temp);
	}

	//This part deletes duplicats in the vector
	sort(invoice_numbers);

	for (int i = 1; i < invoice_numbers.size(); ++i)
	{
		if (invoice_numbers[i - 1] == invoice_numbers[i])
		{
			invoice_numbers.erase(invoice_numbers.begin() + i);
			i--;
		}
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
	while (is >> temp){
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

// Program to move files from one directory to another
void move_invoices(vector<string> invoices)
{
	ofstream missing{ "Missing_Invoices.txt" };

	string N_source = "Y:\\Control\\Invoice Scans\\2014\\Authorised Scans\\"; // N Invoices Source
	string Q_source = "Y:\\Control\\Invoice Scans\\2015\\Authorised Scans 2015\\"; // Q Invoices Source
	string R_source = "Y:\\Control\\Invoice Scans\\2016\\Authorised Scans 2016\\"; // R Invoices Source

	for (int i = 0; i < invoices.size(); ++i)
	{
		string source;
		int result = 0;
		invoices[i].append(".S.pdf");
		
		if (invoices[i][0] == 'Q') 
			source = Q_source;	
		else if (invoices[i][0] == 'R')
			source = R_source;
		else if (invoices[i][0] == 'N')
			source = N_source;
		else return; //Need to place error handling in here

		source.append(invoices[i]);
		result = CopyFile(source, invoices[i]);	 //rename(y, x);
		if (result == 1)
			puts("File successfully copied.");
		else
		{
			remove(invoices[i].c_str());		//Remove missing files that have been copied to fix bug
			missing << invoices[i] << '\n';
		}
	}
}

//	Program to construct email to be sent
void email(const email_list& e,const string& j)
{
	ofstream os{ j+".txt" };

	os << "Hi ";
	for (int i = 0; i < e.first_names.size(); ++i)
	{
		os << e.first_names[i] << ", ";
	}

	os << "/n/nPlease find attached the invoices relating to Jan 16 recharges. /n/nKind Regards,/n/n";
}

//------------------------------------------------------------------------------------------------------------

int main()
{
	vector<string> Invoice_Numbers;				// Stores Invoice Numbers
	string file_name = "Invoice_Finder.txt";	// Txt file to find invoices from
	string job; // Stores the job number
	
	find_invoice_numbers(file_name, Invoice_Numbers);
	
	job = find_job_number(file_name);
	cout << job << "\n";

	for (int i = 0; i < Invoice_Numbers.size(); ++i)
	{
		cout << Invoice_Numbers[i] << '\n';
	}

	move_invoices(Invoice_Numbers);
}