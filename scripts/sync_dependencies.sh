
starting_directory=${PWD}

if [ ! -d "$HOME/i686_cross_compiler" ]
then
  git clone git@github.com:idrol/I686-elf-compiler.git $HOME/i686_cross_compiler
fi

cd $HOME/i686_cross_compiler

missing_dependencies=0

for line in $(cat "$starting_directory/dependency_list.txt")
do
  if ! type "$line" &> /dev/null; then
    echo "Cannot find $line"
    missing_dependencies=1
  fi
done

if [ "$missing_dependencies" -ne 0 ] 
then
  echo "Installing missing dependencies"
  sudo ./install_dependencies.sh
fi

source ./i686.env

if ! type "i686-elf-gcc" > /dev/null;
then
  bash ./install.sh
fi

cd $starting_directory