#!/bin/bash
# This file is only meant to be used to regenerate the files needed for the project

set +e # Disable error handling

# Files to copy
# cp /home/so/trabalho-2022-2023/parte-1/*.sh . # This will delete all the scripts made, be careful
cp /home/so/trabalho-2022-2023/parte-1/*.txt . 
# cp /home/so/trabalho-2022-2023/parte-1/*.def . # Don't run this for the same reason as the above
cp -r /home/so/trabalho-2022-2023/parte-1/so-2022-trab1-validator .

# Files to link
ln -s /home/so/trabalho-2022-2023/parte-1/error
ln -s /home/so/trabalho-2022-2023/parte-1/success
ln -s /home/so/trabalho-2022-2023/parte-1/debug
