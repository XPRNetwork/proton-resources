# Atom Boilerplate for Proton Blockchain

## Directory
- atom: Smart Contracts
- js_tests: JS Tests
- c++_tests: C++ Tests (optional, have to manually build EOSIO)

## Mac OS X:

#### Mac OS X Brew Install
```sh
brew tap eosio/eosio
brew tap eosio/eosio.cdt

brew install eosio
brew install eosio.cdt
```
#### Mac OS X Brew Uninstall
```sh
brew remove eosio
brew remove eosio.cdt
```

## Ubuntu Linux:

#### Ubuntu 18.04 Install
```sh
wget https://github.com/eosio/eos/releases/download/v2.0.7/eosio_2.0.7-1-ubuntu-18.04_amd64.deb
sudo apt install ./eosio_2.0.7-1-ubuntu-18.04_amd64.deb

wget https://github.com/eosio/eosio.cdt/releases/download/v1.7.0/eosio.cdt_1.7.0-1-ubuntu-18.04_amd64.deb
sudo apt install ./eosio.cdt_1.7.0-1-ubuntu-18.04_amd64.deb
```

#### Ubuntu 16.04 Install
```sh
wget https://github.com/eosio/eos/releases/download/v2.0.7/eosio_2.0.7-1-ubuntu-16.04_amd64.deb
sudo apt install ./eosio_2.0.7-1-ubuntu-16.04_amd64.deb

wget https://github.com/eosio/eosio.cdt/releases/download/v1.7.0/eosio.cdt_1.7.0-1-ubuntu-16.04_amd64.deb
sudo apt install ./eosio.cdt_1.7.0-1-ubuntu-16.04_amd64.deb
```
#### Ubuntu Uninstall
```sh
sudo apt remove eosio
sudo apt remove eosio.cdt
```

## RPM-based (CentOS, Amazon Linux, etc.):

#### RPM Install
```sh
wget https://github.com/eosio/eos/releases/download/v2.0.7/eosio-2.0.7-1.el7.x86_64.rpm
sudo yum install ./eosio-2.0.7-1.el7.x86_64.rpm

wget https://github.com/eosio/eosio.cdt/releases/download/v1.7.0/eosio.cdt-1.7.0-1.el7.x86_64.rpm
sudo yum install ./eosio.cdt-1.7.0-1.el7.x86_64.rpm
```
#### RPM Uninstall
```sh
sudo yum remove eosio
sudo yum remove eosio.cdt
```