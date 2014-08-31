Pod::Spec.new do |s|
  s.name          = "ProtocolBuffers"
  s.version       = "2.2.0"
  s.summary       = "Protocol Buffers are a way of encoding structured data in an efficient yet extensible format."
  s.homepage      = "https://github.com/vl4dimir/ProtocolBuffers-ObjC"
  s.license       = 'New BSD'
  s.author        = { "Vladimir Mitrovic" => "snipe714@gmail.com" }
  s.source        = { :git => "https://github.com/vl4dimir/ProtocolBuffers-ObjC", :tag => '2.2.0' }
  s.platform      = :ios, '6.0'
  s.source_files  = 'objectivec/Classes/*.{h,m}'
  s.exclude_files = 'objectivec/Classes/Main.m'
  s.requires_arc  = false
  s.prefix_header_contents = '#import "Bootstrap.h"'
end
