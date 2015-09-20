git checkout version0
./generate_release_build.sh
cd build
make
{ time ./MatchingEngine/NimbusExchange ../data/small_orders.txt ../profiles/version0_small_fills.txt 1 ;} 2> ../profiles/version0_small_stats.txt
cd ..

git checkout version1
./generate_release_build.sh
cd build
make
{ time ./MatchingEngine/NimbusExchange ../data/small_orders.txt ../profiles/version1_small_fills.txt 1 ;} 2> ../profiles/version1_small_stats.txt
cd ..

git checkout version2
./generate_release_build.sh
cd build
make
{ time ./MatchingEngine/NimbusExchange ../data/small_orders.txt ../profiles/version2_small_fills.txt 1 ;} 2> ../profiles/version2_small_stats.txt
cd ..

git checkout master
./generate_release_build.sh
cd build
make
{ time ./MatchingEngine/NimbusExchange ../data/small_orders.txt ../profiles/master_small_fills.txt 1 ;} 2> ../profiles/master_small_stats.txt
cd ..

# --

git checkout version0
./generate_release_build.sh
cd build
make
{ time ./MatchingEngine/NimbusExchange ../data/orders.txt ../profiles/version0_fills.txt 1 ;} 2> ../profiles/version0_stats.txt
cd ..

git checkout version1
./generate_release_build.sh
cd build
make
{ time ./MatchingEngine/NimbusExchange ../data/orders.txt ../profiles/version1_fills.txt 1 ;} 2> ../profiles/version1_stats.txt
cd ..

git checkout version2
./generate_release_build.sh
cd build
make
{ time ./MatchingEngine/NimbusExchange ../data/orders.txt ../profiles/version2_fills.txt 1 ;} 2> ../profiles/version2_stats.txt
cd ..

git checkout master
./generate_release_build.sh
cd build
make
{ time ./MatchingEngine/NimbusExchange ../data/orders.txt ../profiles/master_fills.txt 1 ;} 2> ../profiles/master_stats.txt
cd ..

git checkout master
./generate_release_build.sh
cd build
make
{ time ./MatchingEngine/NimbusExchange ../data/orders.txt ../profiles/master_fills2.txt 2 ;} 2> ../profiles/master_stats2.txt
cd ..

git checkout master
./generate_release_build.sh
cd build
make
{ time ./MatchingEngine/NimbusExchange ../data/orders.txt ../profiles/master_fills3.txt 3 ;} 2> ../profiles/master_stats3.txt
cd ..

git checkout master
./generate_release_build.sh
cd build
make
{ time ./MatchingEngine/NimbusExchange ../data/orders.txt ../profiles/master_fills4.txt 4 ;} 2> ../profiles/master_stats4.txt
cd ..

git checkout master
./generate_release_build.sh
cd build
make
{ time ./MatchingEngine/NimbusExchange ../data/orders.txt ../profiles/master_fills5.txt 5 ;} 2> ../profiles/master_stats5.txt
cd ..

git checkout master
./generate_release_build.sh
cd build
make
{ time ./MatchingEngine/NimbusExchange ../data/orders.txt ../profiles/master_fills6.txt 6 ;} 2> ../profiles/master_stats6.txt
cd ..


