-- This script is used to format the code using clang-format
-- You need to install clang-format and luafilesystem using luarocks to use this script

local lfs = require "lfs"

-- Function for checking that a file exists
function directory_exists(path)
    local attributes = lfs.attributes(path)
    return attributes and attributes.mode == "directory"
end

-- Function for checking that a file exists
function file_exists(path)
    local attributes = lfs.attributes(path)
    return attributes and attributes.mode == "file"
end

-- Function for finding all the files with the specified extensions in the directory and all its subdirectories
function find_files_with_extensions(directory, extensions)
    local files = {}
    for file in lfs.dir(directory) do
        if file ~= "." and file ~= ".." then
            local path = directory .. "/" .. file
            local mode = lfs.attributes(path, "mode")
            if mode == "file" then
                --  Get the file extension
                local file_extension = path:match("%.([^%.]+)$")
                for _, ext in ipairs(extensions) do
                    -- Check if the file extension matches one of the extensions
                    if file_extension == ext then
                        table.insert(files, path)
                        break
                    end
                end
            elseif mode == "directory" then
                -- Recursively find all the files in the subdirectory
                local subfiles = find_files_with_extensions(path, extensions)
                for _, subfile in ipairs(subfiles) do
                    -- Add the subfiles to the files table
                    table.insert(files, subfile)
                end
            end
        end
    end
    return files
end

-- Guarantee the package directory exists
if not (directory_exists("../chip8")) then
    print("Source directory not found!")
    os.exit(74)
end

-- Guarantee the clang-format file exists
if not (file_exists("../.clang-format")) then
    print("Can not find formatting file!")
    os.exit(74)
end

-- The file extensions to format
local extensions = {"c", "cpp", "cc", "h", "hpp", "hh"}
-- Find all the files with the extensions
local files = find_files_with_extensions("../chip8", extensions)
-- Format the files
os.execute("clang-format -i --style=file " .. table.concat(files, " "))
os.exit(0)