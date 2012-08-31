import BoostBuild

# Create a temporary working directory
t = BoostBuild.Tester()

# Create the needed files
t.write("jamroot.jam", "")
t.write("jamfile.jam", """
exe hello : hello.cpp : <linker-script>demo.ld ;
""")
t.write("hello.cpp", """
int main()
{
    return 0;
}

""")
t.write("demo.ld", "")

t.run_build_system()

file_list = BoostBuild.List("bin/$toolset/debug/") * BoostBuild.List("hello.exe hello.obj")
t.expect_addition(file_list)

# Remove temporary directories
t.cleanup()
