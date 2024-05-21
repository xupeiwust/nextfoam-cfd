/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 */

#include "TestSscCommon.h"
#include <adios2.h>
#include <gtest/gtest.h>
#include <mpi.h>
#include <numeric>
#include <thread>

using namespace adios2;
int mpiRank = 0;
int mpiSize = 1;
MPI_Comm mpiComm;

char runMode;

class SscEngineTest : public ::testing::Test
{
public:
    SscEngineTest() = default;
};

void Writer(const Dims &shape, const Dims &start, const Dims &count,
            const size_t steps, const adios2::Params &engineParams,
            const std::string &name)
{
    size_t datasize =
        std::accumulate(count.begin(), count.end(), static_cast<size_t>(1),
                        std::multiplies<size_t>());
    adios2::ADIOS adios(mpiComm);
    adios2::IO io = adios.DeclareIO("WAN");
    io.SetEngine("ssc");
    io.SetParameters(engineParams);
    std::vector<char> myChars(datasize);
    std::vector<unsigned char> myUChars(datasize);
    std::vector<short> myShorts(datasize);
    std::vector<unsigned short> myUShorts(datasize);
    std::vector<int> myInts(datasize);
    std::vector<unsigned int> myUInts(datasize);
    std::vector<float> myFloats(datasize);
    std::vector<double> myDoubles(datasize);
    std::vector<std::complex<float>> myComplexes(datasize);
    std::vector<std::complex<double>> myDComplexes(datasize);
    auto varChars = io.DefineVariable<char>("varChars", shape, start, count);
    auto varUChars =
        io.DefineVariable<unsigned char>("varUChars", shape, start, count);
    auto varShorts = io.DefineVariable<short>("varShorts", shape, start, count);
    auto varUShorts =
        io.DefineVariable<unsigned short>("varUShorts", shape, start, count);
    auto varInts = io.DefineVariable<int>("varInts", shape, start, count);
    auto varUInts =
        io.DefineVariable<unsigned int>("varUInts", shape, start, count);
    auto varFloats = io.DefineVariable<float>("varFloats", shape, start, count);
    auto varDoubles =
        io.DefineVariable<double>("varDoubles", shape, start, count);
    auto varComplexes = io.DefineVariable<std::complex<float>>(
        "varComplexes", shape, start, count);
    auto varDComplexes = io.DefineVariable<std::complex<double>>(
        "varDComplexes", shape, start, count);
    io.DefineAttribute<int>("AttInt", 110);
    adios2::Engine engine = io.Open(name, adios2::Mode::Write);
    engine.LockWriterDefinitions();
    for (size_t i = 0; i < steps; ++i)
    {
        engine.BeginStep();
        GenData(myChars, i, start, count, shape);
        GenData(myUChars, i, start, count, shape);
        GenData(myShorts, i, start, count, shape);
        GenData(myUShorts, i, start, count, shape);
        GenData(myInts, i, start, count, shape);
        GenData(myUInts, i, start, count, shape);
        GenData(myFloats, i, start, count, shape);
        GenData(myDoubles, i, start, count, shape);
        GenData(myComplexes, i, start, count, shape);
        GenData(myDComplexes, i, start, count, shape);
        engine.Put(varChars, myChars.data(), adios2::Mode::Sync);
        engine.Put(varUChars, myUChars.data(), adios2::Mode::Sync);
        engine.Put(varShorts, myShorts.data(), adios2::Mode::Sync);
        engine.Put(varUShorts, myUShorts.data(), adios2::Mode::Sync);
        engine.Put(varInts, myInts.data(), adios2::Mode::Sync);
        engine.Put(varUInts, myUInts.data(), adios2::Mode::Sync);
        engine.Put(varFloats, myFloats.data(), adios2::Mode::Sync);
        engine.Put(varDoubles, myDoubles.data(), adios2::Mode::Sync);
        engine.Put(varComplexes, myComplexes.data(), adios2::Mode::Sync);
        engine.Put(varDComplexes, myDComplexes.data(), adios2::Mode::Sync);
        engine.EndStep();
    }
    engine.Close();
}

void Reader(const Dims &shape, const Dims &start, const Dims &count,
            const size_t steps, const adios2::Params &engineParams,
            const std::string &name)
{
    adios2::ADIOS adios(mpiComm);
    adios2::IO io = adios.DeclareIO("Test");
    io.SetEngine("ssc");
    io.SetParameters(engineParams);
    adios2::Engine engine = io.Open(name, adios2::Mode::Read);
    engine.LockReaderSelections();

    size_t datasize =
        std::accumulate(shape.begin(), shape.end(), static_cast<size_t>(1),
                        std::multiplies<size_t>());
    std::vector<char> myChars(datasize);
    std::vector<unsigned char> myUChars(datasize);
    std::vector<short> myShorts(datasize);
    std::vector<unsigned short> myUShorts(datasize);
    std::vector<int> myInts(datasize);
    std::vector<unsigned int> myUInts(datasize);
    std::vector<float> myFloats(datasize);
    std::vector<double> myDoubles(datasize);
    std::vector<std::complex<float>> myComplexes(datasize);
    std::vector<std::complex<double>> myDComplexes(datasize);

    while (true)
    {
        adios2::StepStatus status = engine.BeginStep(StepMode::Read, 5);
        if (status == adios2::StepStatus::OK)
        {
            const auto &vars = io.AvailableVariables();
            ASSERT_EQ(vars.size(), 10);
            size_t currentStep = engine.CurrentStep();
            adios2::Variable<char> varChars =
                io.InquireVariable<char>("varChars");
            adios2::Variable<unsigned char> varUChars =
                io.InquireVariable<unsigned char>("varUChars");
            adios2::Variable<short> varShorts =
                io.InquireVariable<short>("varShorts");
            adios2::Variable<unsigned short> varUShorts =
                io.InquireVariable<unsigned short>("varUShorts");
            adios2::Variable<int> varInts = io.InquireVariable<int>("varInts");
            adios2::Variable<unsigned int> varUInts =
                io.InquireVariable<unsigned int>("varUInts");
            adios2::Variable<float> varFloats =
                io.InquireVariable<float>("varFloats");
            adios2::Variable<double> varDoubles =
                io.InquireVariable<double>("varDoubles");
            adios2::Variable<std::complex<float>> varComplexes =
                io.InquireVariable<std::complex<float>>("varComplexes");
            adios2::Variable<std::complex<double>> varDComplexes =
                io.InquireVariable<std::complex<double>>("varDComplexes");

            engine.Get(varChars, myChars.data(), adios2::Mode::Sync);
            engine.Get(varUChars, myUChars.data(), adios2::Mode::Sync);
            engine.Get(varShorts, myShorts.data(), adios2::Mode::Sync);
            engine.Get(varUShorts, myUShorts.data(), adios2::Mode::Sync);
            engine.Get(varInts, myInts.data(), adios2::Mode::Sync);
            engine.Get(varUInts, myUInts.data(), adios2::Mode::Sync);
            engine.Get(varFloats, myFloats.data(), adios2::Mode::Sync);
            engine.Get(varDoubles, myDoubles.data(), adios2::Mode::Sync);
            engine.Get(varComplexes, myComplexes.data(), adios2::Mode::Sync);
            engine.Get(varDComplexes, myDComplexes.data(), adios2::Mode::Sync);
            VerifyData(myChars.data(), currentStep, Dims(shape.size(), 0),
                       shape, shape, mpiRank);
            VerifyData(myUChars.data(), currentStep, Dims(shape.size(), 0),
                       shape, shape, mpiRank);
            VerifyData(myShorts.data(), currentStep, Dims(shape.size(), 0),
                       shape, shape, mpiRank);
            VerifyData(myUShorts.data(), currentStep, Dims(shape.size(), 0),
                       shape, shape, mpiRank);
            VerifyData(myInts.data(), currentStep, Dims(shape.size(), 0), shape,
                       shape, mpiRank);
            VerifyData(myUInts.data(), currentStep, Dims(shape.size(), 0),
                       shape, shape, mpiRank);
            VerifyData(myFloats.data(), currentStep, Dims(shape.size(), 0),
                       shape, shape, mpiRank);
            VerifyData(myDoubles.data(), currentStep, Dims(shape.size(), 0),
                       shape, shape, mpiRank);
            VerifyData(myComplexes.data(), currentStep, Dims(shape.size(), 0),
                       shape, shape, mpiRank);
            VerifyData(myDComplexes.data(), currentStep, Dims(shape.size(), 0),
                       shape, shape, mpiRank);
            engine.EndStep();
        }
        else if (status == adios2::StepStatus::EndOfStream)
        {
            std::cout << "[Rank " + std::to_string(mpiRank) +
                             "] SscTest reader end of stream!"
                      << std::endl;
            break;
        }
    }
    engine.Close();
}

TEST_F(SscEngineTest, TestSscMoreReadersThanWriters)
{
    {
        std::string filename = "TestSscMoreReadersThanWriters";
        adios2::Params engineParams = {};

        int worldRank, worldSize;
        Dims start, count, shape;
        MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
        MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
        int mpiGroup;
        int writers = 2;
        if (worldSize < 3)
        {
            return;
        }
        if (worldRank < writers)
        {
            mpiGroup = 0;
        }
        else
        {
            mpiGroup = 1;
        }

        MPI_Comm_split(MPI_COMM_WORLD, mpiGroup, worldRank, &mpiComm);

        MPI_Comm_rank(mpiComm, &mpiRank);
        MPI_Comm_size(mpiComm, &mpiSize);

        size_t steps = 20;

        if (mpiGroup == 0)
        {
            shape = {(size_t)writers, 10};
            start = {(size_t)mpiRank, 0};
            count = {1, 10};
            Writer(shape, start, count, steps, engineParams, filename);
        }

        if (mpiGroup == 1)
        {
            shape = {(size_t)writers, 10};
            start = {0, 0};
            count = shape;
            Reader(shape, start, shape, steps, engineParams, filename);
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }

    {
        std::string filename = "TestSscMoreReadersThanWritersNaive";
        adios2::Params engineParams = {{"Verbose", "0"},
                                       {"EngineMode", "naive"}};

        int worldRank, worldSize;
        Dims start, count, shape;
        MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
        MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
        int mpiGroup;
        int writers = 2;
        if (worldSize < 3)
        {
            return;
        }
        if (worldRank < writers)
        {
            mpiGroup = 0;
        }
        else
        {
            mpiGroup = 1;
        }

        MPI_Comm_split(MPI_COMM_WORLD, mpiGroup, worldRank, &mpiComm);

        MPI_Comm_rank(mpiComm, &mpiRank);
        MPI_Comm_size(mpiComm, &mpiSize);

        size_t steps = 20;

        if (mpiGroup == 0)
        {
            shape = {(size_t)writers, 10};
            start = {(size_t)mpiRank, 0};
            count = {1, 10};
            Writer(shape, start, count, steps, engineParams, filename);
        }

        if (mpiGroup == 1)
        {
            shape = {(size_t)writers, 10};
            start = {0, 0};
            count = shape;
            Reader(shape, start, shape, steps, engineParams, filename);
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int worldRank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();

    MPI_Finalize();
    return result;
}
