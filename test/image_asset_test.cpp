#include <rive/file.hpp>
#include <rive/node.hpp>
#include <rive/shapes/clipping_shape.hpp>
#include <rive/shapes/rectangle.hpp>
#include <rive/shapes/image.hpp>
#include <rive/assets/image_asset.hpp>
#include <rive/relative_local_asset_resolver.hpp>
#include "no_op_factory.hpp"
#include "no_op_renderer.hpp"
#include "rive_file_reader.hpp"
#include <catch.hpp>
#include <cstdio>

TEST_CASE("image assets loads correctly", "[assets]") {
    RenderObjectLeakChecker checker;
    auto file = ReadRiveFile("../../test/assets/walle.riv");

    auto node = file->artboard()->find("walle");
    REQUIRE(node != nullptr);
    REQUIRE(node->is<rive::Image>());
    auto walle = node->as<rive::Image>();
    REQUIRE(walle->imageAsset() != nullptr);
    REQUIRE(walle->imageAsset()->decodedByteSize == 218873);

    auto eve_left = file->artboard()->find("eve_left");
    REQUIRE(eve_left != nullptr);
    REQUIRE(eve_left->is<rive::Image>());
    REQUIRE(eve_left->as<rive::Image>()->imageAsset() != nullptr);
    REQUIRE(eve_left->as<rive::Image>()->imageAsset()->decodedByteSize == 246825);

    auto eve_right = file->artboard()->find("eve_right");
    REQUIRE(eve_right != nullptr);
    REQUIRE(eve_right->is<rive::Image>());
    REQUIRE(eve_right->as<rive::Image>()->imageAsset() != nullptr);
    REQUIRE(eve_right->as<rive::Image>()->imageAsset() != walle->imageAsset());
    REQUIRE(eve_right->as<rive::Image>()->imageAsset() ==
            eve_left->as<rive::Image>()->imageAsset());

    file->artboard()->updateComponents();

    rive::NoOpRenderer renderer;
    file->artboard()->draw(&renderer);
}

TEST_CASE("out of band image assets loads correctly", "[assets]") {
    RenderObjectLeakChecker checker;
    rive::NoOpFactory gEmptyFactory;
    
    std::string filename = "../../test/assets/out_of_band/walle.riv";
    rive::RelativeLocalAssetResolver resolver(filename, &gEmptyFactory);

    auto file = ReadRiveFile(filename.c_str(), &gEmptyFactory, &resolver);

    auto node = file->artboard()->find("walle");
    REQUIRE(node != nullptr);
    REQUIRE(node->is<rive::Image>());
    auto walle = node->as<rive::Image>();
    REQUIRE(walle->imageAsset() != nullptr);
    REQUIRE(walle->imageAsset()->decodedByteSize == 218873);

    auto eve_left = file->artboard()->find("eve_left");
    REQUIRE(eve_left != nullptr);
    REQUIRE(eve_left->is<rive::Image>());
    REQUIRE(eve_left->as<rive::Image>()->imageAsset() != nullptr);
    REQUIRE(eve_left->as<rive::Image>()->imageAsset()->decodedByteSize == 246825);

    auto eve_right = file->artboard()->find("eve_right");
    REQUIRE(eve_right != nullptr);
    REQUIRE(eve_right->is<rive::Image>());
    REQUIRE(eve_right->as<rive::Image>()->imageAsset() != nullptr);
    REQUIRE(eve_right->as<rive::Image>()->imageAsset() != walle->imageAsset());
    REQUIRE(eve_right->as<rive::Image>()->imageAsset() ==
            eve_left->as<rive::Image>()->imageAsset());

    file->artboard()->updateComponents();

    rive::NoOpRenderer renderer;
    file->artboard()->draw(&renderer);
}
