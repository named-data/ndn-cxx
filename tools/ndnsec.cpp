/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 * BSD license, See the LICENSE file for more information
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#include "ndnsec-util.hpp"
#include "ndnsec-list.hpp"
#include "ndnsec-get-default.hpp"
#include "ndnsec-set-default.hpp"
#include "ndnsec-key-gen.hpp"
#include "ndnsec-dsk-gen.hpp"
#include "ndnsec-sign-req.hpp"
#include "ndnsec-cert-gen.hpp"
#include "ndnsec-cert-dump.hpp"
#include "ndnsec-cert-install.hpp"
#include "ndnsec-export.hpp"
#include "ndnsec-import.hpp"
#include "ndnsec-delete.hpp"
#include "ndnsec-sig-verify.hpp"
#include "ndnsec-set-acl.hpp"
#include "ndnsec-unlock-tpm.hpp"
#include "ndnsec-op-tool.hpp"

using namespace ndn;

std::string ndnsec_helper("\
  help         Show all commands.\n\
  list         Display information in PublicInfo.\n\
  get-default  Get default setting info.\n\
  set-default  Configure default setting.\n\
  key-gen      Generate a Key-Signing-Key for an identity.\n\
  sign-req     Generate a certificate signing request.\n\
  cert-gen     Generate an identity certificate.\n\
  cert-dump    Dump a certificate from PublicInfo.\n\
  cert-install Install a certificate into PublicInfo.\n\
  delete       Delete identity/key/certificate.\n\
  export       Export an identity package.\n\
  import       Import an identity package.\n\
  sig-verify   Verify the signature of a Data packet.\n\
  set-acl      Configure ACL of a private key.\n\
  unlock-tpm   Unlock Tpm.\n\
  op-tool      Operator tool.\n\
");

int
main(int argc, char** argv)
{
  if (argc < 2)
    {
      std::cerr << ndnsec_helper << std::endl;
      return 1;
    }

  std::string command(argv[1]);

  try
    {
      if (command == "help")              { std::cerr << ndnsec_helper << std::endl; }
      else if (command == "list")         { return ndnsec_list(argc - 1, argv + 1); }
      else if (command == "get-default")  { return ndnsec_get_default(argc - 1, argv + 1); }
      else if (command == "set-default")  { return ndnsec_set_default(argc - 1, argv + 1); }
      else if (command == "key-gen")      { return ndnsec_key_gen(argc - 1, argv + 1); }
      else if (command == "sign-req")     { return ndnsec_sign_req(argc - 1, argv + 1); }
      else if (command == "cert-gen")     { return ndnsec_cert_gen(argc - 1, argv + 1); }
      else if (command == "cert-dump")    { return ndnsec_cert_dump(argc - 1, argv + 1); }
      else if (command == "cert-install") { return ndnsec_cert_install(argc - 1, argv + 1); }
      else if (command == "delete")       { return ndnsec_delete(argc - 1, argv + 1); }
      else if (command == "export")       { return ndnsec_export(argc - 1, argv + 1); }
      else if (command == "import")       { return ndnsec_import(argc - 1, argv + 1); }
      else if (command == "sig-verify")   { return ndnsec_sig_verify(argc - 1, argv + 1); }
      else if (command == "set-acl")      { return ndnsec_set_acl(argc - 1, argv + 1); }
      else if (command == "unlock-tpm")   { return ndnsec_unlock_tpm(argc - 1, argv + 1); }
      else if (command == "op-tool")      { return ndnsec_op_tool(argc - 1, argv + 1); }
      else {
        std::cerr << ndnsec_helper << std::endl;
        return 1;
      }
    }
  catch (const std::runtime_error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }

  return 0;
}
