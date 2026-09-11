//
//$Id: cache_sim.cpp,v 1.7 2026/08/29 11:12:33 venkatnarayan.h Exp venkatnarayan.h $
/*
* Copyright (c) 2026, Shiv Nadar University, Delhi NCR, India. All Rights
* Reserved. Permission to use, copy, modify and distribute this software for
* educational, research, and not-for-profit purposes, without fee and without a
* signed license agreement, is hereby granted, provided that this paragraph and
* the following two paragraphs appear in all copies, modifications, and
* distributions.
*
* IN NO EVENT SHALL SHIV NADAR UNIVERSITY BE LIABLE TO ANY PARTY FOR DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST
* PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE.
*
* SHIV NADAR UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT
* NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS PROVIDED "AS IS". SHIV
* NADAR UNIVERSITY HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
* ENHANCEMENTS, OR MODIFICATIONS.*/

#include "cache_sim.h"

int main(int argc, char** argv);

namespace snucs {

	void usage(string progname)
	{
		cout <<
			"Usage: \n" <<
			"  " << progname << " -ap <Access patterns file> -c <Config file> -o <Output file>\n";
	}


	void LoadParms(string config_filenm, parameters* parms)
	{
		map<string, string> config_kvs;
		if (!vhutil_config::readConfigFile(config_filenm, config_kvs))
			exit(1);

		for (auto const& kv : config_kvs) {  //Loop thru KV pairs map
			try {
#define MCFD_IF_KEY_COND(k,T) \
  (kv.first == #k) { \
    sciToNumber<T>(kv.second, parms->k, kv.first.c_str()); \
  }

				if MCFD_IF_KEY_COND(NumCacheLevelsExclMM, size_t)
				else if MCFD_IF_KEY_COND(CapacityOfL1Cache, size_t)
				else if MCFD_IF_KEY_COND(CapacityOfL2Cache, size_t)
				else if MCFD_IF_KEY_COND(CapacityOfL3Cache, size_t)
				else if MCFD_IF_KEY_COND(CapacityOfL4Cache, size_t)
				else if (kv.first == "WritePolicy") {
					if (kv.second == "WB") parms->WritePolicy = Write_Policy::WRITE_BACK;
					if (kv.second == "WT") parms->WritePolicy = Write_Policy::WRITE_THRU;
				}
				else if (kv.first == "ReplacementPolicy") {
					if (kv.second == "LRU") parms->ReplacementPolicy = Replacement_Policy::LRU;
					if (kv.second == "FIFO") parms->ReplacementPolicy = Replacement_Policy::FIFO;
				}
				else {
					std::cerr << "-E-: " << "Unrecognized key '" << kv.first << "'" << std::endl;
					exit(1);
				}
			}
			catch (std::string& e) {
				std::cerr << "-E-: " << e << std::endl;
				exit(1);
			}
		}  //End loop thru KV pairs map

		//Check that all necessary members are set
		if (parms->WritePolicy == Write_Policy::INVALID) {
			printf("-E-: WritePolicy has not been specified in the config file\n");
			exit(1);
		}

		if (parms->ReplacementPolicy == Replacement_Policy::INVALID) {
			printf("-E-: ReplacementPolicy has not been specified in the config file\n");
			exit(1);
		}

		switch (parms->NumCacheLevelsExclMM)
		{
		case 0:
			printf("-E-: NumCacheLevelsExclMM has not been specified in the config file, or is 0 which is invalid\n");
			exit(1);

		case 1:
			if (parms->CapacityOfL1Cache && !parms->CapacityOfL2Cache &&
				!parms->CapacityOfL3Cache && !parms->CapacityOfL4Cache) {
			}
			else {
				printf("-E-: CapacityOfL1Cache (only) must be specified in the config file\n");
				exit(1);
			}
			break;

		case 2:
			if (parms->CapacityOfL1Cache && parms->CapacityOfL2Cache &&
				!parms->CapacityOfL3Cache && !parms->CapacityOfL4Cache) {
			}
			else {
				printf("-E-: Capacities of L1 and L2 caches (only) must be specified in the config file\n");
				exit(1);
			}
			break;

		case 3:
			if (parms->CapacityOfL1Cache && parms->CapacityOfL2Cache &&
				parms->CapacityOfL3Cache && !parms->CapacityOfL4Cache) {
			}
			else {
				printf("-E-: Capacities of L1, L2 and L3 caches (only) must be specified in the config file\n");
				exit(1);
			}
			break;

		case 4:
			if (parms->CapacityOfL1Cache && parms->CapacityOfL2Cache &&
				parms->CapacityOfL3Cache && parms->CapacityOfL4Cache) {
			}
			else {
				printf("-E-: Capacities of L1, L2, L3 and L4 caches (only) must be specified in the config file\n");
				exit(1);
			}
			break;

		default:
			printf("-E-: Invalid value for NumCacheLevelsExclMM in the config file\n");
			exit(1);
		}

		printf("-I-: Done reading config file, printing parameters read ...\n");
		PrintConfigFileDataStruct(parms);  //@@Debug
		printf("-I-: Done printing parameters read.\n\n");
	}


	template <typename T> void sciToNumber(const string& str, T& val,
		const char* context)
	{
		try {
			val = boost::lexical_cast<T>(str);
		}
		catch (boost::bad_lexical_cast&) {
			std::string s = "-E-: Unable to format \"";
			s += str;
			s += "\" as a number (";
			s += std::string(context);
			s += ")\n";
			cout << s;
			exit(1);
		}
	}


	void PrintConfigFileDataStruct(parameters* parms)
	{
		printf("parms->NumCacheLevelsExclMM = %zu\n", parms->NumCacheLevelsExclMM);
		printf("parms->WritePolicy = %s\n",
			parms->WritePolicy == Write_Policy::WRITE_BACK ? "WB" :
			parms->WritePolicy == Write_Policy::WRITE_THRU ? "WT" :
			"INVALID");
		printf("parms->ReplacementPolicy = %s\n",
			parms->ReplacementPolicy == Replacement_Policy::LRU ? "LRU" :
			parms->ReplacementPolicy == Replacement_Policy::FIFO ? "FIFO" :
			"INVALID");
		printf("parms->CapacityOfL1Cache = %zu\n", parms->CapacityOfL1Cache);
		printf("parms->CapacityOfL2Cache = %zu\n", parms->CapacityOfL2Cache);
		printf("parms->CapacityOfL3Cache = %zu\n", parms->CapacityOfL3Cache);
		printf("parms->CapacityOfL4Cache = %zu\n", parms->CapacityOfL4Cache);
	}


	void PrintAccessPatternFileDataStruct(vector<MemAccess>& m_accesses)
	{
		for (const auto& access : m_accesses) {
			printf("%c %zu", access.read ? 'R' : 'W', access.addr);
			if (!access.read)
				printf(" %d", access.write_data);
			printf("\n");
		}
	}


	void LoadAccessPatterns(string access_pat_filenm, vector<MemAccess>& m_accesses)
	{
		std::ifstream file(access_pat_filenm);

		if (!file)
		{
			std::cerr << "Cannot open '" << access_pat_filenm << "'\n";
			exit(1);
		}

		std::string line;

		while (std::getline(file, line))
		{
			if (line.empty()) continue;

			std::istringstream iss(line);

			char read_write;
			MemAccess m_access;

			if (iss >> read_write >> m_access.addr)
			{
				if (read_write == 'R') {
					m_access.read = true;
					m_access.write_data = -99999;
				}
				else if (read_write == 'W') {
					m_access.read = false;
					iss >> m_access.write_data;
				}
				else {
					cerr << "-W-: Invalid read/write flag, skipping ...\n";
					continue;
				}

				/* @@Debug
				cout << "read_flag = " << m_access.read
					 << ", addr = " << m_access.addr
					 << ", write_data = " << m_access.write_data
					 << '\n';
				*/

				m_accesses.push_back(m_access);
			}
		}

		printf("-I-: Done reading access patterns, printing contents ...\n");
		PrintAccessPatternFileDataStruct(m_accesses);   //@@Debug
		printf("-I-: Done printing access patterns read.\n\n");
	}


	void SimulateCache(vector<MemAccess>& m_accesses, parameters* parms, string &output_file)
	{
		MainMemory mm;
		vector<Cache> caches;

		for (size_t n = 0; n < parms->NumCacheLevelsExclMM; n++) {
			switch (n) {
			case 0:
				caches.push_back(Cache(parms->CapacityOfL1Cache));
				break;
			case 1:
				caches.push_back(Cache(parms->CapacityOfL2Cache));
				break;
			case 2:
				caches.push_back(Cache(parms->CapacityOfL3Cache));
				break;
			case 3:
				caches.push_back(Cache(parms->CapacityOfL4Cache));
				break;
			default:
				throw("-E-: Invalid cache level");
			}
		}

		for (size_t n = 0; n < parms->NumCacheLevelsExclMM - 1; n++) {
			caches[n].setLowerLevelCache(&caches[n + 1], &mm);
			caches[n + 1].setUpperLevelCache(&caches[n], &mm);
		}

		caches[0].setAsHighestLevelCache();

		std::ofstream file(output_file);
		if (!file.is_open())
			throw("-E-: Could not open output file for writing!");

		size_t access_num = 0;
		for (const auto& access : m_accesses)
		{
			vector<vector<int>> level_indices_modified_in_curr_access;	//Index 0 is for L1 cache, 1 for L2 cache, etc. The vector at each
							//index can have multiple entries if multiple addresses in that cache level are written to in the same access.
			level_indices_modified_in_curr_access.resize(parms->NumCacheLevelsExclMM);

			if (access.read)
			{
				//Read
				int data = caches[0].read(access.addr, parms->WritePolicy, parms->ReplacementPolicy, level_indices_modified_in_curr_access);
			}
			else
			{
				//Write
				caches[0].write(access.addr, access.write_data, parms->WritePolicy, parms->ReplacementPolicy, level_indices_modified_in_curr_access);
			}

			file << ++access_num << ".\t";
			for (size_t n = 0; n < caches.size(); n++) {
				caches[n].printContents(level_indices_modified_in_curr_access[n], file, false);
				if (n < caches.size() - 1)	//Print an extra tab after each cache level (except the last one).
					file << "\t";
			}
			file << "\n";
		}
	}

} //End namespace snucs


int main(int argc, char** argv)
{
	string progname(argv[0]);
	string access_pat_filenm;
	string config_filenm;
	string output_file;
	parameters parms_struct = {};
	parameters* parms = &parms_struct;

	if (argc == 7) {
		for (int i = 1; i < argc; ++i) {
			std::string arg = argv[i];

			if (arg == "-c" && i + 1 < argc)
				config_filenm = argv[++i];
			else if (arg == "-ap" && i + 1 < argc)
				access_pat_filenm = argv[++i];
			else if (arg == "-o" && i + 1 < argc)
				output_file = argv[++i];
		}
	}
	else {
		snucs::usage(progname);
		return 1;
	}

	snucs::LoadParms(config_filenm, parms);

	cout << "access_pat_filenm: " << access_pat_filenm << "\n";
	cout << "config_filenm: " << config_filenm << "\n";

	vector<MemAccess> m_accesses;
	snucs::LoadAccessPatterns(access_pat_filenm, m_accesses);

	snucs::SimulateCache(m_accesses, parms, output_file);

	return 0;
}

