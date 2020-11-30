# Atom Boilerplate for Proton Blockchain

## Directory
- atom: Smart Contracts
- frontend: Frontend UI for Smart Contract
- js_tests: JS Tests
- c++_tests: C++ Tests (optional, have to manually build EOSIO)

## Install
<details>
  <summary>Mac OS X</summary>
  <p>
    brew tap eosio/eosio
    brew tap eosio/eosio.cdt

    brew install eosio
    brew install eosio.cdt
  </p>
</details>

<details>
  <summary>Ubuntu 18.04</summary>
  <p>
    wget https://github.com/eosio/eos/releases/download/v2.0.7/eosio_2.0.7-1-ubuntu-18.04_amd64.deb
    sudo apt install ./eosio_2.0.7-1-ubuntu-18.04_amd64.deb

    wget https://github.com/eosio/eosio.cdt/releases/download/v1.7.0/eosio.cdt_1.7.0-1-ubuntu-18.04_amd64.deb
    sudo apt install ./eosio.cdt_1.7.0-1-ubuntu-18.04_amd64.deb
  </p>
</details>
<details>
  <summary>Ubuntu 16.04</summary>
  <p>
    wget https://github.com/eosio/eos/releases/download/v2.0.7/eosio_2.0.7-1-ubuntu-16.04_amd64.deb
    sudo apt install ./eosio_2.0.7-1-ubuntu-16.04_amd64.deb

    wget https://github.com/eosio/eosio.cdt/releases/download/v1.7.0/eosio.cdt_1.7.0-1-ubuntu-16.04_amd64.deb
    sudo apt install ./eosio.cdt_1.7.0-1-ubuntu-16.04_amd64.deb
  </p>
</details>

<details>
  <summary>RPM-based (CentOS, Amazon Linux, etc.)</summary>
  <p>
    wget https://github.com/eosio/eos/releases/download/v2.0.7/eosio-2.0.7-1.el7.x86_64.rpm
    sudo yum install ./eosio-2.0.7-1.el7.x86_64.rpm

    wget https://github.com/eosio/eosio.cdt/releases/download/v1.7.0/eosio.cdt-1.7.0-1.el7.x86_64.rpm
    sudo yum install ./eosio.cdt-1.7.0-1.el7.x86_64.rpm
  </p>
</details>


## Uninstall

<details>
  <summary>Mac OS X</summary>
  <p>
    brew remove eosio
    brew remove eosio.cdt
  </p>
</details>

<details>
  <summary>Ubuntu 16.04/18.04</summary>
  <p>
    sudo apt remove eosio
    sudo apt remove eosio.cdt
  </p>
</details>


<details>
  <summary>RPM-based (CentOS, Amazon Linux, etc.)</summary>
  <p>
    sudo yum remove eosio
    sudo yum remove eosio.cdt
  </p>
</details>
