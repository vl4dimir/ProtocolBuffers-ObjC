Pod::Spec.new do |s|
  s.name         = "ProtocolBuffers"
  s.version      = "1.0.1"
  s.summary      = "Protocol Buffers are a way of encoding structured data in an efficient yet extensible format. Google uses Protocol Buffers for almost all of its internal RPC protocols and file formats."
  s.homepage     = "https://github.com/k06a/ABStackedRollView"
  s.license      = 'New BSD'
  s.author       = { "Anton Bukov" => "k06aaa@gmail.com" }
  s.source       = { :git => "https://github.com/k06a/ProtocolBuffers-ObjC", :tag => '2.2.0' }
  s.platform     = :ios, '6.0'
  s.source_files = 'objectivec/Classes/*.{h,m}'
  s.prefix_header_contents = '#import "Bootstrap.h"'
  s.requires_arc = false
end
